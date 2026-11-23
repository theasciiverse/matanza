//! Joining a friend's game: a built-in SSH client, so no `ssh` is needed.
//! A server's key is remembered the first time (in `known_hosts` in the
//! state folder) and a changed key is refused, as ssh does.

use crate::term::{self, Keys, RawScreen};
use anyhow::{bail, Context, Result};
use russh::client;
use russh::keys::{known_hosts, PublicKeyOrCertificate};
use russh::ChannelMsg;
use std::path::PathBuf;
use std::sync::Arc;
use tokio::signal::unix::{signal, SignalKind};

/// `host`, `host:port` or `[v6]:port`; the port is 2022 unless given.
pub fn parse_address(s: &str) -> Result<(String, u16)> {
    let s = s.trim();
    if s.is_empty() {
        bail!("no address given");
    }
    if let Some(rest) = s.strip_prefix('[') {
        let (h, p) = rest.split_once(']').context("a [ without its ]")?;
        let port = match p.strip_prefix(':') {
            Some(p) => p.parse().context("that port is not a number")?,
            None => crate::host::FIRST_PORT,
        };
        return Ok((h.to_string(), port));
    }
    match s.rsplit_once(':') {
        Some((h, p)) if !h.contains(':') => Ok((h.to_string(), p.parse().context("that port is not a number")?)),
        _ => Ok((s.to_string(), crate::host::FIRST_PORT)),
    }
}

struct Client {
    host: String,
    port: u16,
    known: PathBuf,
    changed: Arc<std::sync::Mutex<bool>>,
}

impl client::Handler for Client {
    type Error = anyhow::Error;

    async fn check_server_key(&mut self, key: &PublicKeyOrCertificate) -> Result<bool, Self::Error> {
        let PublicKeyOrCertificate::PublicKey { key, .. } = key else {
            return Ok(false);
        };
        match known_hosts::check_known_hosts_path(&self.host, self.port, key, &self.known) {
            Ok(true) => Ok(true),
            Ok(false) => {
                known_hosts::learn_known_hosts_path(&self.host, self.port, key, &self.known)?;
                log::info!("remembered the key of {}:{}", self.host, self.port);
                Ok(true)
            }
            Err(e) => {
                log::warn!("the key of {}:{} has changed: {}", self.host, self.port, e);
                *self.changed.lock().unwrap() = true;
                Ok(false)
            }
        }
    }
}

/// Play in a friend's game at `address` until it ends.
pub async fn join(state: &std::path::Path, address: &str) -> Result<()> {
    let (host, port) = parse_address(address)?;
    let changed = Arc::new(std::sync::Mutex::new(false));
    let handler = Client { host: host.clone(), port, known: state.join("known_hosts"), changed: changed.clone() };
    let config = Arc::new(client::Config {
        inactivity_timeout: None,
        keepalive_interval: Some(std::time::Duration::from_secs(15)),
        nodelay: true,
        ..Default::default()
    });
    log::info!("joining {}:{}", host, port);
    let connect = client::connect(config, (host.as_str(), port), handler);
    let mut session = match tokio::time::timeout(std::time::Duration::from_secs(10), connect).await {
        Err(_) => bail!("{}:{} does not answer", host, port),
        Ok(Err(e)) => {
            if *changed.lock().unwrap() {
                bail!(
                    "{}:{} is not the game it was last time (its key has changed), so it was not joined. If your friend set it up again, remove its line from {}",
                    host,
                    port,
                    state.join("known_hosts").display()
                );
            }
            bail!("could not reach {}:{}: {}", host, port, e)
        }
        Ok(Ok(s)) => s,
    };
    let user = std::env::var("USER").unwrap_or_else(|_| "player".into());
    if !session.authenticate_none(user).await?.success() {
        bail!("{}:{} did not let us in", host, port);
    }
    let mut channel = session.channel_open_session().await?;
    let (cols, rows) = term::size();
    channel.request_pty(false, "xterm", cols as u32, rows as u32, 0, 0, &[]).await?;
    channel.request_shell(false).await?;

    let _screen = RawScreen::enter()?;
    let keys = Keys::new()?;
    let mut winch = signal(SignalKind::window_change())?;
    let mut kbuf = [0u8; 256];
    loop {
        tokio::select! {
            r = keys.read(&mut kbuf) => {
                let n = r?;
                if n == 0 { break; }
                log::debug!("keys {:?}", &kbuf[..n]);
                channel.data(&kbuf[..n]).await?;
            }
            m = channel.wait() => match m {
                Some(ChannelMsg::Data { data }) => term::show(&data)?,
                Some(ChannelMsg::Eof) | Some(ChannelMsg::Close) | None => break,
                Some(ChannelMsg::ExitStatus { .. }) => {}
                _ => {}
            },
            _ = winch.recv() => {
                let (c, r) = term::size();
                log::debug!("window {}x{}", c, r);
                channel.window_change(c as u32, r as u32, 0, 0).await?;
            }
        }
    }
    let _ = session.disconnect(russh::Disconnect::ByApplication, "", "en").await;
    log::info!("left {}:{}", host, port);
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::parse_address;

    #[test]
    fn addresses() {
        assert_eq!(parse_address("box").unwrap(), ("box".into(), 2022));
        assert_eq!(parse_address("192.168.1.4:2023").unwrap(), ("192.168.1.4".into(), 2023));
        assert_eq!(parse_address("[fe80::1]:2024").unwrap(), ("fe80::1".into(), 2024));
        assert_eq!(parse_address("fe80::1").unwrap(), ("fe80::1".into(), 2022));
        assert!(parse_address("box:port").is_err());
    }
}
