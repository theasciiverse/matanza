//! Where things live, and the game server process.

use anyhow::{bail, Context, Result};
use std::path::{Path, PathBuf};
use std::process::Stdio;
use tokio::process::{Child, Command};

/// `$XDG_STATE_HOME/matanza`, or `~/.local/state/matanza`: the host key, the
/// servers remembered when joining, the log and the game's socket.
pub fn state_dir() -> Result<PathBuf> {
    let base = match std::env::var_os("XDG_STATE_HOME") {
        Some(d) if !d.is_empty() => PathBuf::from(d),
        _ => {
            let home = std::env::var_os("HOME").context("HOME is not set")?;
            if cfg!(target_os = "macos") {
                PathBuf::from(home).join("Library/Application Support")
            } else {
                PathBuf::from(home).join(".local/state")
            }
        }
    };
    let dir = base.join("matanza");
    std::fs::create_dir_all(&dir).with_context(|| format!("creating {}", dir.display()))?;
    #[cfg(unix)]
    {
        use std::os::unix::fs::PermissionsExt;
        let _ = std::fs::set_permissions(&dir, std::fs::Permissions::from_mode(0o700));
    }
    Ok(dir)
}

/// The game server, `matanza-server`, beside this program.
fn server_binary() -> Result<PathBuf> {
    if let Some(p) = std::env::var_os("MATANZA_SERVER") {
        return Ok(PathBuf::from(p));
    }
    let exe = std::env::current_exe().context("finding this program")?;
    let dir = exe.parent().context("finding this program's folder")?;
    let name = if cfg!(windows) { "matanza-server.exe" } else { "matanza-server" };
    let p = dir.join(name);
    if !p.exists() {
        bail!("{} is missing; it belongs beside {}", name, exe.display());
    }
    Ok(p)
}

/// The game's socket: in the state folder, unless that path is too long for
/// a Unix socket (about 100 bytes), then in a private folder of the runtime
/// folder or of /tmp.
fn socket_path(state: &Path) -> Result<PathBuf> {
    let name = format!("game-{}.sock", std::process::id());
    let p = state.join(&name);
    if p.as_os_str().len() < 100 {
        return Ok(p);
    }
    let base = std::env::var_os("XDG_RUNTIME_DIR").map(PathBuf::from).unwrap_or_else(|| PathBuf::from("/tmp"));
    let dir = base.join(format!("matanza-{}", unsafe { libc::getuid() }));
    std::fs::create_dir_all(&dir)?;
    #[cfg(unix)]
    {
        use std::os::unix::fs::{MetadataExt, PermissionsExt};
        std::fs::set_permissions(&dir, std::fs::Permissions::from_mode(0o700))?;
        if std::fs::metadata(&dir)?.uid() != unsafe { libc::getuid() } {
            bail!("{} belongs to someone else", dir.display());
        }
    }
    Ok(dir.join(name))
}

/// A running game server, stopped when dropped.
pub struct Server {
    child: Child,
    pub socket: PathBuf,
}

impl Server {
    /// Start a server with `ships` computer ships, listening on a socket in
    /// the state folder, and wait until it takes connections.
    pub async fn start(state: &Path, ships: u32) -> Result<Server> {
        let socket = socket_path(state)?;
        let _ = std::fs::remove_file(&socket);
        let log = std::fs::File::create(state.join("server.log")).context("creating server.log")?;
        let mut cmd = Command::new(server_binary()?);
        cmd.arg(format!("--socket={}", socket.display()))
            .arg(format!("--computer-ships={}", ships))
            .stdin(Stdio::null())
            .stdout(log.try_clone()?)
            .stderr(log)
            .kill_on_drop(true);
        #[cfg(target_os = "linux")]
        unsafe {
            // The server goes when we go, even if we are killed.
            cmd.pre_exec(|| {
                libc::prctl(libc::PR_SET_PDEATHSIG, libc::SIGTERM);
                Ok(())
            });
        }
        log::info!("starting the game server, {} computer ships, socket {}", ships, socket.display());
        let mut child = cmd.spawn().context("starting matanza-server")?;
        for _ in 0..100 {
            if socket.exists() {
                // bound; it listens straight after binding
                tokio::time::sleep(std::time::Duration::from_millis(30)).await;
                return Ok(Server { child, socket });
            }
            if let Some(status) = child.try_wait()? {
                bail!("the game server stopped at once ({status}); see server.log in {}", state.display());
            }
            tokio::time::sleep(std::time::Duration::from_millis(50)).await;
        }
        let _ = child.start_kill();
        bail!("the game server did not start; see server.log in {}", state.display())
    }

    pub async fn stop(mut self) {
        #[cfg(unix)]
        if let Some(pid) = self.child.id() {
            unsafe {
                libc::kill(pid as i32, libc::SIGTERM);
            }
            if tokio::time::timeout(std::time::Duration::from_secs(2), self.child.wait()).await.is_ok() {
                let _ = std::fs::remove_file(&self.socket);
                return;
            }
        }
        let _ = self.child.kill().await;
        let _ = std::fs::remove_file(&self.socket);
    }
}
