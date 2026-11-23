//! Playing in this terminal: straight onto the game's socket, no network.

use crate::telnet::{self, Telnet};
use crate::term::{self, Keys, RawScreen};
use anyhow::{Context, Result};
use std::path::Path;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::UnixStream;
use tokio::signal::unix::{signal, SignalKind};

/// Play until the game says goodbye or the player leaves it.
pub async fn play(socket: &Path) -> Result<()> {
    let stream = UnixStream::connect(socket).await.context("connecting to the game")?;
    let (mut from_game, mut to_game) = stream.into_split();
    let _screen = RawScreen::enter()?;
    let keys = Keys::new()?;
    let mut winch = signal(SignalKind::window_change())?;

    let (cols, rows) = term::size();
    let mut tn = Telnet::new(cols, rows);
    log::info!("local play starts at {}x{}", cols, rows);

    let mut kbuf = [0u8; 256];
    let mut gbuf = vec![0u8; 65536];
    loop {
        tokio::select! {
            r = keys.read(&mut kbuf) => {
                let n = r?;
                if n == 0 { break; }
                log::debug!("keys {:?}", &kbuf[..n]);
                let mut out = Vec::with_capacity(n + 4);
                telnet::to_game(&kbuf[..n], &mut out);
                if to_game.write_all(&out).await.is_err() { break; }
            }
            r = from_game.read(&mut gbuf) => {
                let n = r.unwrap_or(0);
                if n == 0 { break; }
                let (mut screen, mut reply) = (Vec::new(), Vec::new());
                tn.from_game(&gbuf[..n], &mut screen, &mut reply);
                if !reply.is_empty() && to_game.write_all(&reply).await.is_err() { break; }
                if !screen.is_empty() { term::show(&screen)?; }
            }
            _ = winch.recv() => {
                let (cols, rows) = term::size();
                log::info!("window {}x{}", cols, rows);
                if let Some(naws) = tn.resize(cols, rows) {
                    if to_game.write_all(&naws).await.is_err() { break; }
                }
            }
        }
    }
    log::info!("local play ends");
    Ok(())
}
