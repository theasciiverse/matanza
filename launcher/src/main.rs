//! Matanza's launcher: one program that plays and serves.
//!
//! With no options it opens a menu: play here against computer ships, host
//! a game on the local network (over SSH), or join a friend's. The game
//! itself is `matanza-server` beside this program, Alejandro Forero Cuervo's
//! C server, which only listens on a Unix socket in the state folder.

mod game;
mod host;
mod join;
mod local;
mod telnet;
mod term;

use anyhow::{bail, Context, Result};
use crossterm::event::{self, Event, KeyCode, KeyEventKind, KeyModifiers};
use crossterm::{cursor, style, terminal, QueueableCommand};
use std::io::Write;
use std::path::Path;

const USAGE: &str = "\
Matanza: the multiplayer, action, network-based, real time, ASCII art based,
spaceship, slogan-lacking game.

Usage: matanza [OPTION]...

With no option, a menu opens.

  --play             play here against computer ships
  --host             play here, with the game open to your local network
  --join HOST[:PORT] join a friend's game (port 2022 unless given)
  --server           open a game to your local network without playing in it
  --ships N          computer ships in a game with one person (default 3, up
                     to 8); each friend who joins takes the place of one
  --port N           the port to host on (default 2022 or the next free one)
  --help             show this and exit
  --version          show the version and exit

Friends can also join with plain ssh: ssh -p 2022 HOST
Files: the host key, remembered servers, settings and logs are in
$XDG_STATE_HOME/matanza (~/.local/state/matanza).
";

#[derive(Clone, Copy, PartialEq)]
enum Mode {
    Menu,
    Play,
    Host,
    Server,
    Join,
}

struct Options {
    mode: Mode,
    join: Option<String>,
    ships: Option<u32>,
    port: Option<u16>,
}

fn parse_args() -> Result<Options> {
    let mut o = Options { mode: Mode::Menu, join: None, ships: None, port: None };
    let mut args = std::env::args().skip(1);
    while let Some(a) = args.next() {
        let (name, inline) = match a.split_once('=') {
            Some((n, v)) if n.starts_with("--") => (n.to_string(), Some(v.to_string())),
            _ => (a.clone(), None),
        };
        let mut value = |what: &str| -> Result<String> {
            match inline.clone() {
                Some(v) => Ok(v),
                None => args.next().with_context(|| format!("{} needs {}", name, what)),
            }
        };
        match name.as_str() {
            "--play" | "-play" => o.mode = Mode::Play,
            "--host" | "-host" => o.mode = Mode::Host,
            "--server" | "-server" => o.mode = Mode::Server,
            "--join" | "-join" => {
                o.mode = Mode::Join;
                o.join = Some(value("an address")?);
            }
            "--ships" | "-ships" => {
                let n: u32 = value("a number")?.parse().context("--ships needs a number")?;
                o.ships = Some(n.min(8));
            }
            "--port" | "-port" => {
                o.port = Some(value("a number")?.parse().context("--port needs a number")?);
            }
            "--help" | "-h" | "-help" => {
                print!("{}", USAGE);
                std::process::exit(0);
            }
            "--version" | "-v" | "-version" => {
                println!("matanza {}", env!("CARGO_PKG_VERSION"));
                std::process::exit(0);
            }
            _ => bail!("unknown option {} (try --help)", a),
        }
    }
    Ok(o)
}

/// What the menu remembers between runs.
struct Settings {
    ships: u32,
    join: String,
}

impl Settings {
    fn load(state: &Path) -> Settings {
        let mut s = Settings { ships: 3, join: String::new() };
        if let Ok(text) = std::fs::read_to_string(state.join("settings")) {
            for line in text.lines() {
                match line.split_once('=') {
                    Some(("ships", v)) => s.ships = v.trim().parse().unwrap_or(3).min(8),
                    Some(("join", v)) => s.join = v.trim().to_string(),
                    _ => {}
                }
            }
        }
        s
    }

    fn save(&self, state: &Path) {
        let _ = std::fs::write(state.join("settings"), format!("ships={}\njoin={}\n", self.ships, self.join));
    }
}

fn init_log(state: &Path) {
    let level = std::env::var("RUST_LOG").unwrap_or_else(|_| "info".into());
    let mut b = env_logger::Builder::new();
    b.parse_filters(&level);
    if let Ok(f) = std::fs::File::create(state.join("matanza.log")) {
        b.target(env_logger::Target::Pipe(Box::new(f)));
    }
    let _ = b.try_init();
}

#[tokio::main]
async fn main() {
    if let Err(e) = run().await {
        eprintln!("matanza: {:#}", e);
        std::process::exit(1);
    }
}

async fn run() -> Result<()> {
    let opts = parse_args()?;
    let state = game::state_dir()?;
    init_log(&state);
    log::info!("matanza {} starting", env!("CARGO_PKG_VERSION"));
    let mut settings = Settings::load(&state);
    if let Some(n) = opts.ships {
        settings.ships = n;
    }
    match opts.mode {
        Mode::Menu => menu(&state, &mut settings, opts.port).await,
        Mode::Play => play_here(&state, settings.ships).await,
        Mode::Host => host_and_play(&state, settings.ships, opts.port).await.map(|_| ()),
        Mode::Server => serve(&state, settings.ships, opts.port).await,
        Mode::Join => join::join(&state, opts.join.as_deref().unwrap_or("")).await,
    }
}

async fn play_here(state: &Path, ships: u32) -> Result<()> {
    let server = game::Server::start(state, ships).await?;
    let r = local::play(&server.socket).await;
    server.stop().await;
    r
}

/// The lines that tell friends how to join.
fn join_lines(port: u16) -> Vec<String> {
    let addrs = host::lan_addresses();
    let mut lines = Vec::new();
    if addrs.is_empty() {
        lines.push("This machine has no address on a local network right now.".into());
        return lines;
    }
    let p = if port == host::FIRST_PORT { String::new() } else { format!(":{}", port) };
    for a in &addrs {
        lines.push(format!("matanza --join {}{}", a, p));
    }
    lines.push(format!("or with plain ssh: ssh -p {} {}", port, addrs[0]));
    lines
}

/// Host and play here; returns whether the hosted game ran.
async fn host_and_play(state: &Path, ships: u32, port: Option<u16>) -> Result<bool> {
    let server = game::Server::start(state, ships).await?;
    let host = match host::start(state, server.socket.clone(), port).await {
        Ok(h) => h,
        Err(e) => {
            server.stop().await;
            return Err(e);
        }
    };
    let mut lines = vec![
        format!("Your game is open on the local network, port {}.", host.port),
        String::new(),
        "Friends join from the Matanza menu (Join a friend) or with".into(),
    ];
    lines.extend(join_lines(host.port).into_iter().map(|l| format!("    {}", l)));
    lines.push(String::new());
    lines.push("Enter to fly, Esc to close the game again.".into());
    let go = notice("Hosting", &lines, true)?;
    let r = if go { local::play(&server.socket).await } else { Ok(()) };
    host.stop();
    server.stop().await;
    r.map(|_| go)
}

/// A game open to the local network, with nobody playing here.
async fn serve(state: &Path, ships: u32, port: Option<u16>) -> Result<()> {
    let server = game::Server::start(state, ships).await?;
    let host = host::start(state, server.socket.clone(), port).await?;
    println!("Matanza is open on the local network, port {}. Join with", host.port);
    for l in join_lines(host.port) {
        println!("    {}", l);
    }
    println!("Ctrl-C stops it.");
    let mut tick = tokio::time::interval(std::time::Duration::from_secs(1));
    loop {
        tokio::select! {
            _ = tokio::signal::ctrl_c() => break,
            _ = tick.tick() => log::debug!("{} playing over SSH", host.players.load(std::sync::atomic::Ordering::SeqCst)),
        }
    }
    host.stop();
    server.stop().await;
    Ok(())
}

// ---------------------------------------------------------------- the menu

const ITEMS: [&str; 5] = ["Play against computer ships", "Host a game on your network", "Join a friend", "Computer ships", "Quit"];

struct MenuScreen;

impl MenuScreen {
    fn enter() -> Result<MenuScreen> {
        terminal::enable_raw_mode()?;
        let mut out = std::io::stdout();
        out.queue(terminal::EnterAlternateScreen)?.queue(cursor::Hide)?;
        out.flush()?;
        Ok(MenuScreen)
    }
}

impl Drop for MenuScreen {
    fn drop(&mut self) {
        let mut out = std::io::stdout();
        let _ = out.queue(cursor::Show);
        let _ = out.queue(terminal::LeaveAlternateScreen);
        let _ = out.flush();
        let _ = terminal::disable_raw_mode();
    }
}

const LOGO: [&str; 6] = [
    r" __  __    _    _____    _    _   _ _____   _    ",
    r"|  \/  |  / \  |_   _|  / \  | \ | |__  /  / \   ",
    r"| |\/| | / _ \   | |   / _ \ |  \| | / /  / _ \  ",
    r"| |  | |/ ___ \  | |  / ___ \| |\  |/ /_ / ___ \ ",
    r"|_|  |_/_/   \_\ |_| /_/   \_\_| \_/____/_/   \_\",
    r"",
];

fn put(out: &mut impl Write, x: u16, y: u16, text: &str) -> Result<()> {
    out.queue(cursor::MoveTo(x, y))?.queue(style::Print(text))?;
    Ok(())
}

fn centred(out: &mut impl Write, cols: u16, y: u16, text: &str) -> Result<()> {
    let w = text.chars().count() as u16;
    put(out, cols.saturating_sub(w) / 2, y, text)
}

fn draw_menu(sel: usize, settings: &Settings, message: &str) -> Result<()> {
    let (cols, rows) = term::size();
    let mut out = std::io::stdout().lock();
    out.queue(terminal::Clear(terminal::ClearType::All))?;
    let top = rows.saturating_sub(22) / 2;
    for (i, l) in LOGO.iter().enumerate() {
        out.queue(style::SetForegroundColor(style::Color::Red))?;
        centred(&mut out, cols, top + i as u16, l)?;
    }
    out.queue(style::ResetColor)?;
    centred(&mut out, cols, top + 6, "the multiplayer, action, network-based, real time,")?;
    centred(&mut out, cols, top + 7, "ASCII art based, spaceship, slogan-lacking game")?;
    let left = cols.saturating_sub(34) / 2;
    for (i, item) in ITEMS.iter().enumerate() {
        let label = if i == 3 { format!("{}: < {} >", item, settings.ships) } else { item.to_string() };
        let y = top + 10 + i as u16 * 2;
        if i == sel {
            out.queue(style::SetAttribute(style::Attribute::Reverse))?;
            put(&mut out, left, y, &format!(" {:<32} ", label))?;
            out.queue(style::SetAttribute(style::Attribute::Reset))?;
        } else {
            put(&mut out, left, y, &format!(" {:<32} ", label))?;
        }
    }
    let hint = match sel {
        3 => "Left and Right: how many computer ships fly against you",
        _ => "Up and Down to choose, Enter to start, Q to quit",
    };
    centred(&mut out, cols, top + 21, hint)?;
    if !message.is_empty() {
        out.queue(style::SetForegroundColor(style::Color::Yellow))?;
        for (i, line) in wrap(message, cols.saturating_sub(4) as usize).iter().enumerate() {
            centred(&mut out, cols, top + 23 + i as u16, line)?;
        }
        out.queue(style::ResetColor)?;
    }
    out.flush()?;
    Ok(())
}

fn wrap(text: &str, width: usize) -> Vec<String> {
    let mut lines = Vec::new();
    let mut line = String::new();
    for w in text.split_whitespace() {
        if !line.is_empty() && line.len() + 1 + w.len() > width.max(20) {
            lines.push(std::mem::take(&mut line));
        }
        if !line.is_empty() {
            line.push(' ');
        }
        line.push_str(w);
    }
    if !line.is_empty() {
        lines.push(line);
    }
    lines
}

fn key() -> Result<(KeyCode, KeyModifiers)> {
    loop {
        if let Event::Key(k) = event::read()? {
            if k.kind != KeyEventKind::Release {
                log::debug!("menu key {:?}", k);
                return Ok((k.code, k.modifiers));
            }
        }
    }
}

/// A screen of lines; returns true for Enter, false for Esc.
fn notice(title: &str, lines: &[String], ask: bool) -> Result<bool> {
    let _screen = MenuScreen::enter()?;
    loop {
        let (cols, rows) = term::size();
        {
            let mut out = std::io::stdout().lock();
            out.queue(terminal::Clear(terminal::ClearType::All))?;
            let top = rows.saturating_sub(lines.len() as u16 + 2) / 2;
            out.queue(style::SetAttribute(style::Attribute::Bold))?;
            centred(&mut out, cols, top, title)?;
            out.queue(style::SetAttribute(style::Attribute::Reset))?;
            let w = lines.iter().map(|l| l.chars().count()).max().unwrap_or(0) as u16;
            for (i, l) in lines.iter().enumerate() {
                put(&mut out, cols.saturating_sub(w) / 2, top + 2 + i as u16, l)?;
            }
            out.flush()?;
        }
        if !ask {
            return Ok(true);
        }
        match key()? {
            (KeyCode::Enter, _) | (KeyCode::Char(' '), _) => return Ok(true),
            (KeyCode::Esc, _) | (KeyCode::Char('q'), _) => return Ok(false),
            (KeyCode::Char('c'), m) if m.contains(KeyModifiers::CONTROL) => return Ok(false),
            _ => {}
        }
    }
}

/// Ask for a friend's address on the menu screen.
fn ask_address(settings: &Settings) -> Result<Option<String>> {
    let _screen = MenuScreen::enter()?;
    let mut text = settings.join.clone();
    loop {
        let (cols, rows) = term::size();
        {
            let mut out = std::io::stdout().lock();
            out.queue(terminal::Clear(terminal::ClearType::All))?;
            let top = rows.saturating_sub(8) / 2;
            out.queue(style::SetAttribute(style::Attribute::Bold))?;
            centred(&mut out, cols, top, "Join a friend")?;
            out.queue(style::SetAttribute(style::Attribute::Reset))?;
            centred(&mut out, cols, top + 2, "Your friend's address, as their screen shows it")?;
            centred(&mut out, cols, top + 3, "(the port is 2022 unless you add one, as in 192.168.1.20:2023)")?;
            let field = format!("> {}_", text);
            put(&mut out, cols.saturating_sub(40) / 2, top + 5, &field)?;
            centred(&mut out, cols, top + 7, "Enter to join, Esc to go back")?;
            out.flush()?;
        }
        match key()? {
            (KeyCode::Enter, _) => {
                return Ok(if text.trim().is_empty() { None } else { Some(text.trim().to_string()) });
            }
            (KeyCode::Esc, _) => return Ok(None),
            (KeyCode::Char('c'), m) if m.contains(KeyModifiers::CONTROL) => return Ok(None),
            (KeyCode::Backspace, _) => {
                text.pop();
            }
            (KeyCode::Char(c), _) if text.len() < 60 && (c.is_ascii_alphanumeric() || ".:-[]_".contains(c)) => {
                text.push(c)
            }
            _ => {}
        }
    }
}

async fn menu(state: &Path, settings: &mut Settings, port: Option<u16>) -> Result<()> {
    let mut sel = 0usize;
    let mut message = String::new();
    loop {
        let choice = {
            let _screen = MenuScreen::enter()?;
            loop {
                draw_menu(sel, settings, &message)?;
                match key()? {
                    (KeyCode::Up, _) | (KeyCode::Char('k'), _) => sel = (sel + ITEMS.len() - 1) % ITEMS.len(),
                    (KeyCode::Down, _) | (KeyCode::Char('j'), _) | (KeyCode::Tab, _) => sel = (sel + 1) % ITEMS.len(),
                    (KeyCode::Left, _) | (KeyCode::Char('h'), _) if sel == 3 => {
                        settings.ships = settings.ships.saturating_sub(1)
                    }
                    (KeyCode::Right, _) | (KeyCode::Char('l'), _) if sel == 3 => {
                        settings.ships = (settings.ships + 1).min(8)
                    }
                    (KeyCode::Enter, _) | (KeyCode::Char(' '), _) if sel != 3 => break sel,
                    (KeyCode::Char('q'), _) | (KeyCode::Esc, _) => break 4,
                    (KeyCode::Char('c'), m) if m.contains(KeyModifiers::CONTROL) => break 4,
                    _ => {}
                }
            }
        };
        settings.save(state);
        message.clear();
        let result = match choice {
            0 => play_here(state, settings.ships).await,
            1 => host_and_play(state, settings.ships, port).await.map(|_| ()),
            2 => match ask_address(settings)? {
                Some(a) => {
                    settings.join = a.clone();
                    settings.save(state);
                    join::join(state, &a).await
                }
                None => Ok(()),
            },
            _ => return Ok(()),
        };
        if let Err(e) = result {
            log::warn!("{:#}", e);
            message = format!("{:#}", e);
        }
    }
}
