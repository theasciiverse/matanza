//! Hosting on the local network: an SSH server in front of the game.
//!
//! Anyone on the local network may join, with no password (a name is asked
//! in the game). Each SSH session with a terminal gets its own connection to
//! the game's Unix socket; the bytes go both ways, and window changes become
//! telnet NAWS for the game. Connections from outside the local network are
//! turned away.

use crate::telnet::{self, Telnet};
use anyhow::{Context, Result};
use russh::keys::{Algorithm, PrivateKey};
use russh::server::{Auth, ChannelOpenHandle, Handle, Msg, Server as _, Session};
use russh::{Channel, ChannelId, MethodKind, MethodSet};
use std::net::{IpAddr, SocketAddr};
use std::path::{Path, PathBuf};
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::{TcpListener, UnixStream};
use tokio::sync::mpsc;

pub const FIRST_PORT: u16 = 2022;
pub const LAST_PORT: u16 = 2031;

/// The host key, made in the state folder the first time.
pub fn host_key(state: &Path) -> Result<PrivateKey> {
    let path = state.join("host_key");
    if path.exists() {
        return russh::keys::load_secret_key(&path, None).with_context(|| format!("reading {}", path.display()));
    }
    let key = PrivateKey::random(&mut rand::rng(), Algorithm::Ed25519).context("making a host key")?;
    let text = key.to_openssh(russh::keys::ssh_key::LineEnding::LF).context("writing the host key")?;
    #[cfg(unix)]
    {
        use std::io::Write;
        use std::os::unix::fs::OpenOptionsExt;
        let mut f = std::fs::OpenOptions::new().write(true).create_new(true).mode(0o600).open(&path)?;
        f.write_all(text.as_bytes())?;
    }
    log::info!("made a host key in {}", path.display());
    Ok(key)
}

/// Is this address on the local network (or this machine)?
pub fn local_network(ip: IpAddr) -> bool {
    match ip {
        IpAddr::V4(v4) => {
            let o = v4.octets();
            v4.is_loopback()
                || v4.is_private()
                || v4.is_link_local()
                || (o[0] == 100 && (o[1] & 0xc0) == 64) // shared address space (CGNAT, some VPNs)
        }
        IpAddr::V6(v6) => {
            if let Some(v4) = v6.to_ipv4_mapped() {
                return local_network(IpAddr::V4(v4));
            }
            let s = v6.segments();
            v6.is_loopback() || (s[0] & 0xfe00) == 0xfc00 || (s[0] & 0xffc0) == 0xfe80
        }
    }
}

/// A game open to the local network.
pub struct Host {
    pub port: u16,
    pub players: Arc<AtomicUsize>,
    shutdown: russh::server::RunningServerHandle,
}

impl Host {
    pub fn stop(&self) {
        self.shutdown.shutdown("The game has ended.".into());
    }
}

/// Listen on the first free port from `port` up to 2031 (or only `port`,
/// if it was asked for), and pass every session to the game at `socket`.
pub async fn start(state: &Path, socket: PathBuf, port: Option<u16>) -> Result<Host> {
    let key = host_key(state)?;
    let (first, last) = match port {
        Some(p) => (p, p),
        None => (FIRST_PORT, LAST_PORT),
    };
    let mut listener = None;
    for p in first..=last {
        match TcpListener::bind(("0.0.0.0", p)).await {
            Ok(l) => {
                listener = Some((l, p));
                break;
            }
            Err(e) => log::info!("port {} is taken: {}", p, e),
        }
    }
    let (listener, port) = listener.with_context(|| {
        if first == last {
            format!("port {} is taken", first)
        } else {
            format!("ports {} to {} are all taken", first, last)
        }
    })?;

    let config = Arc::new(russh::server::Config {
        methods: MethodSet::from(&[MethodKind::None][..]),
        auth_rejection_time: std::time::Duration::from_millis(300),
        auth_rejection_time_initial: Some(std::time::Duration::from_millis(0)),
        inactivity_timeout: None,
        keepalive_interval: Some(std::time::Duration::from_secs(15)),
        keys: vec![key],
        nodelay: true,
        ..Default::default()
    });
    let players = Arc::new(AtomicUsize::new(0));
    let mut server = HostServer { socket, players: players.clone() };
    let (tx, rx) = tokio::sync::oneshot::channel();
    tokio::spawn(async move {
        let running = server.run_on_socket(config, &listener);
        let _ = tx.send(running.handle());
        if let Err(e) = running.await {
            log::warn!("the SSH server stopped: {}", e);
        }
    });
    let shutdown = rx.await.context("starting the SSH server")?;
    log::info!("hosting on port {}", port);
    Ok(Host { port, players, shutdown })
}

#[derive(Clone)]
struct HostServer {
    socket: PathBuf,
    players: Arc<AtomicUsize>,
}

impl russh::server::Server for HostServer {
    type Handler = Player;

    fn new_client(&mut self, peer: Option<SocketAddr>) -> Player {
        let lan = peer.map(|a| local_network(a.ip())).unwrap_or(false);
        log::info!("SSH connection from {:?}{}", peer, if lan { "" } else { ", not on the local network: refused" });
        Player {
            socket: self.socket.clone(),
            players: self.players.clone(),
            peer,
            lan,
            size: (80, 24),
            channel: None,
            to_game: None,
        }
    }
}

enum ToGame {
    Keys(Vec<u8>),
    Resize(u16, u16),
}

struct Player {
    socket: PathBuf,
    players: Arc<AtomicUsize>,
    peer: Option<SocketAddr>,
    lan: bool,
    size: (u16, u16),
    channel: Option<ChannelId>,
    to_game: Option<mpsc::UnboundedSender<ToGame>>,
}

impl Player {
    fn start_game(&mut self, channel: ChannelId, handle: Handle) {
        if self.to_game.is_some() {
            return;
        }
        let (tx, rx) = mpsc::unbounded_channel();
        self.to_game = Some(tx);
        let socket = self.socket.clone();
        let size = self.size;
        let peer = self.peer;
        let players = self.players.clone();
        tokio::spawn(async move {
            players.fetch_add(1, Ordering::SeqCst);
            log::info!("{:?} joins the game at {}x{}", peer, size.0, size.1);
            if let Err(e) = relay(&socket, size, rx, &handle, channel).await {
                log::info!("{:?}: {}", peer, e);
            }
            players.fetch_sub(1, Ordering::SeqCst);
            log::info!("{:?} leaves the game", peer);
            let _ = handle.exit_status_request(channel, 0).await;
            let _ = handle.eof(channel).await;
            let _ = handle.close(channel).await;
        });
    }
}

/// One player's bytes between their SSH channel and the game.
async fn relay(
    socket: &Path,
    size: (u16, u16),
    mut rx: mpsc::UnboundedReceiver<ToGame>,
    handle: &Handle,
    channel: ChannelId,
) -> Result<()> {
    let stream = UnixStream::connect(socket).await.context("the game is not running")?;
    let (mut from_game, mut to_game) = stream.into_split();
    let mut tn = Telnet::new(size.0, size.1);
    let mut buf = vec![0u8; 65536];
    loop {
        tokio::select! {
            r = from_game.read(&mut buf) => {
                let n = r.unwrap_or(0);
                if n == 0 { return Ok(()); }
                let (mut screen, mut reply) = (Vec::new(), Vec::new());
                tn.from_game(&buf[..n], &mut screen, &mut reply);
                if !reply.is_empty() { if to_game.write_all(&reply).await.is_err() { return Ok(()); } }
                if !screen.is_empty() && handle.data(channel, screen).await.is_err() {
                    return Ok(());
                }
            }
            m = rx.recv() => match m {
                None => return Ok(()),
                Some(ToGame::Keys(k)) => {
                    log::debug!("keys {:?}", k);
                    let mut out = Vec::with_capacity(k.len() + 4);
                    telnet::to_game(&k, &mut out);
                    if to_game.write_all(&out).await.is_err() { return Ok(()); }
                }
                Some(ToGame::Resize(c, r)) => {
                    log::debug!("window {}x{}", c, r);
                    if let Some(naws) = tn.resize(c, r) {
                        if to_game.write_all(&naws).await.is_err() { return Ok(()); }
                    }
                }
            }
        }
    }
}

fn clamp_size(c: u32, r: u32) -> (u16, u16) {
    let c = if c == 0 { 80 } else { c.min(1000) };
    let r = if r == 0 { 24 } else { r.min(1000) };
    (c as u16, r as u16)
}

impl russh::server::Handler for Player {
    type Error = anyhow::Error;

    async fn auth_none(&mut self, _user: &str) -> Result<Auth, Self::Error> {
        Ok(if self.lan { Auth::Accept } else { Auth::reject() })
    }

    async fn channel_open_session(
        &mut self,
        channel: Channel<Msg>,
        reply: ChannelOpenHandle,
        _session: &mut Session,
    ) -> Result<(), Self::Error> {
        if !self.lan || self.channel.is_some() {
            drop(reply);
            return Ok(());
        }
        self.channel = Some(channel.id());
        reply.accept().await;
        Ok(())
    }

    async fn pty_request(
        &mut self,
        channel: ChannelId,
        _term: &str,
        col_width: u32,
        row_height: u32,
        _pix_width: u32,
        _pix_height: u32,
        _modes: &[(russh::Pty, u32)],
        session: &mut Session,
    ) -> Result<(), Self::Error> {
        self.size = clamp_size(col_width, row_height);
        session.channel_success(channel)?;
        Ok(())
    }

    async fn shell_request(&mut self, channel: ChannelId, session: &mut Session) -> Result<(), Self::Error> {
        session.channel_success(channel)?;
        self.start_game(channel, session.handle());
        Ok(())
    }

    async fn exec_request(
        &mut self,
        channel: ChannelId,
        _data: &[u8],
        session: &mut Session,
    ) -> Result<(), Self::Error> {
        // whatever was asked for, the game is what runs here
        session.channel_success(channel)?;
        self.start_game(channel, session.handle());
        Ok(())
    }

    async fn data(&mut self, _channel: ChannelId, data: &[u8], _session: &mut Session) -> Result<(), Self::Error> {
        if let Some(tx) = &self.to_game {
            let _ = tx.send(ToGame::Keys(data.to_vec()));
        }
        Ok(())
    }

    async fn window_change_request(
        &mut self,
        _channel: ChannelId,
        col_width: u32,
        row_height: u32,
        _pix_width: u32,
        _pix_height: u32,
        _session: &mut Session,
    ) -> Result<(), Self::Error> {
        let (c, r) = clamp_size(col_width, row_height);
        self.size = (c, r);
        if let Some(tx) = &self.to_game {
            let _ = tx.send(ToGame::Resize(c, r));
        }
        Ok(())
    }

    async fn channel_eof(&mut self, _channel: ChannelId, _session: &mut Session) -> Result<(), Self::Error> {
        self.to_game = None;
        Ok(())
    }

    async fn channel_close(&mut self, _channel: ChannelId, _session: &mut Session) -> Result<(), Self::Error> {
        self.to_game = None;
        Ok(())
    }
}

/// This machine's addresses on the local network, for the join line.
pub fn lan_addresses() -> Vec<IpAddr> {
    let mut out = Vec::new();
    #[cfg(unix)]
    unsafe {
        let mut ifap: *mut libc::ifaddrs = std::ptr::null_mut();
        if libc::getifaddrs(&mut ifap) == 0 {
            let mut p = ifap;
            while !p.is_null() {
                let a = (*p).ifa_addr;
                if !a.is_null() && (*a).sa_family as i32 == libc::AF_INET {
                    let sin = a as *const libc::sockaddr_in;
                    let ip = std::net::Ipv4Addr::from(u32::from_be((*sin).sin_addr.s_addr));
                    if !ip.is_loopback() && local_network(IpAddr::V4(ip)) && !out.contains(&IpAddr::V4(ip)) {
                        out.push(IpAddr::V4(ip));
                    }
                }
                p = (*p).ifa_next;
            }
            libc::freeifaddrs(ifap);
        }
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn only_the_local_network() {
        for a in ["192.168.1.5", "10.0.0.2", "172.16.4.4", "127.0.0.1", "169.254.1.1", "100.64.0.1", "::1", "fe80::1", "fd00::5", "::ffff:192.168.0.9"] {
            assert!(local_network(a.parse().unwrap()), "{a}");
        }
        for a in ["8.8.8.8", "172.32.0.1", "100.128.0.1", "2001:db8::1", "::ffff:1.1.1.1"] {
            assert!(!local_network(a.parse().unwrap()), "{a}");
        }
    }
}
