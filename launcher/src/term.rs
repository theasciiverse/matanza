//! The local terminal during a game: raw mode, the keys as bytes, and size
//! changes.

use anyhow::Result;
use std::io::Write;
use std::os::fd::RawFd;
use tokio::io::unix::AsyncFd;

/// Raw mode on the alternate screen for as long as it lives.
pub struct RawScreen;

impl RawScreen {
    pub fn enter() -> Result<RawScreen> {
        crossterm::terminal::enable_raw_mode()?;
        let mut out = std::io::stdout();
        crossterm::execute!(out, crossterm::terminal::EnterAlternateScreen)?;
        Ok(RawScreen)
    }
}

impl Drop for RawScreen {
    fn drop(&mut self) {
        let mut out = std::io::stdout();
        // the game hides the cursor and sets colours; put both back
        let _ = out.write_all(b"\x1b[0m\x1b[?25h");
        let _ = crossterm::execute!(out, crossterm::terminal::LeaveAlternateScreen);
        let _ = crossterm::terminal::disable_raw_mode();
    }
}

/// The keyboard read without a blocking thread, so nothing is left waiting
/// on it when the game ends. It reads its own handle on the terminal, so
/// making it non-blocking leaves standard output as it was.
pub struct Keys {
    fd: AsyncFd<std::fs::File>,
}

impl Keys {
    pub fn new() -> Result<Keys> {
        use std::os::fd::AsRawFd;
        let tty = std::fs::OpenOptions::new().read(true).open("/dev/tty")?;
        let raw: RawFd = tty.as_raw_fd();
        unsafe {
            let flags = libc::fcntl(raw, libc::F_GETFL);
            libc::fcntl(raw, libc::F_SETFL, flags | libc::O_NONBLOCK);
        }
        // SAFETY: the File owns its descriptor for as long as the AsyncFd lives.
        let fd = unsafe { AsyncFd::register(tty) }.map_err(std::io::Error::from)?;
        Ok(Keys { fd })
    }

    pub async fn read(&self, buf: &mut [u8]) -> std::io::Result<usize> {
        use std::io::Read;
        loop {
            let mut guard = self.fd.readable().await?;
            match guard.try_io(|f| (&*f.get_ref()).read(buf)) {
                Ok(result) => return result,
                Err(_would_block) => continue,
            }
        }
    }
}

/// The terminal's size, columns and rows.
pub fn size() -> (u16, u16) {
    crossterm::terminal::size().unwrap_or((80, 24))
}

/// Write to the terminal straight away.
pub fn show(bytes: &[u8]) -> std::io::Result<()> {
    let mut out = std::io::stdout().lock();
    out.write_all(bytes)?;
    out.flush()
}
