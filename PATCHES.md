# Changes from upstream

This is the Asciiverse packaging of Matanza, Alejandro Forero Cuervo's
multiplayer ASCII space battle (Freaks Unidos, 2000 to 2003, GPL-2.0 or
later; bachue.com/matanza, now in the Wayback Machine). The branch
`asciiverse` is built up in three layers, one commit per change:

1. **Upstream 0.13**, as Debian imports it: the `upstream` branch of the
   Debian Games Team's repository (remote `debian`,
   salsa.debian.org/games-team/matanza), tag `upstream/0.13+ds2`. Debian's
   repack only drops the generated `matanza.c` and `ai.c`.
2. **Debian's six patches** from matanza 0.13+ds2-3 (the upload of
   2026-11-22), one commit each with its own author and date, tagged
   `debian/0.13+ds2-3-patches`. Debian's own tag `debian/0.13+ds2-3`
   points at Debian's packaging commit and is left as it is.
3. **Our changes**, below.

The later 0.15 release (2003-11-14) is kept on the branch `upstream-0.15`,
tag `0.15`.

## Why 0.13 and not 0.15

0.13 is the game Debian has kept building and playing since 2004: one
CWEB file, a fixed set of screens, the telnet front it was made for, and
with Debian's six patches it builds cleanly with today's GCC. 0.15 is a
rewrite in progress ("Complete code rewrite. Many things might have been
broken", says its NEWS): an embedded Scheme interpreter for configuration,
a widget system, menus for teams and universes. It does not build with
today's compilers as released (it needs gnu89 inline semantics,
`-fcommon`, the libpng 1.5 fix Debian made for 0.13, and declarations it
leaves out), and once built it does not start: every universe has to be
written in its Scheme, and the release ships none. It has no computer
ships either. So 0.13 is the base, and Debian's fixes apply to it
unchanged.

## Debian's patches (layer 2)

| Patch | Author | What |
|---|---|---|
| 01-fix-verbose-macro | Frank Lichtenheld, 2010 | the VERBOSE macro and an array bounds warning |
| 02-typos | Peter Pentchev, 2010 | a typo |
| 03-cppcheck | Peter Pentchev, 2011 | a cppcheck false positive in Expat |
| 04-format | Bart Martens, 2012 | a format string, for GCC 4.7 and hardening (bug 675295) |
| 05-fix-build-with-libpng1.5 | Nobuhiro Iwamatsu, 2012 | libpng 1.5 and later (bug 636984) |
| 06-gcc14 | Patrice Duroux, 2024 | GCC 14 (bug 1075255) |

## Our changes (layer 3)

- **Family-friendly ship names and quit-cancel quip.** The name offered to
  a new player is drawn at random from a list that held Hitler, Stalin,
  Laid, Bastard, Butthead and Beavis. They are gone; in their places (two
  lines of `@<Select a random name@>`, now `mat_name_pick`) are six names
  of ours, Nova, Comet, Quasar, Pulsar, Orion and Vega. One of the quips
  for backing out of quitting, "Good. Now kill them all, you bastard!", is
  now ours: "Good. Now go blast them all, hotshot!". The original README
  loses a joke about using the chat for net.sex, and "give the bastard a
  lesson" becomes "give them a lesson".
- **No telnet on the network.** The server listened for telnet players on
  TCP port 7993 and for MMP machines on 7793, on every interface, and went
  into the background. Now it stays in the foreground and listens only on
  a Unix domain socket named with `--socket=PATH` (required), made with
  mode 0600 and removed when the server stops. `--port` and
  `--port-machine` are gone and there is no TCP listener in the server to
  turn back on; the MMP listener is not opened at all. Players come in
  through the launcher (below); telnet stays on the socket, for the
  terminal type and window size. With no address to show, join and leave
  messages show the name alone.
- **Computer pilots.** Matanza was waiting for computer ships: its README
  promised them, and MMP was designed so a program could fly a ship from
  outside the server. Debian's `matanza-ai` is the start of that, but it
  only adds one ship named AICode that never moves: MMP has no message
  that steers a ship, and its frames send positions with the high byte
  lost. So the pilots are a new section of the server, *Computer pilots*,
  where they see the universe whole. `--computer-ships=N` (up to 12) keeps
  N of them flying while one person plays; each further person takes the
  place of one ("Fidel makes way and jumps out"), and when the last person
  leaves they go too. A pilot is a connection with no socket and one ship,
  named from the players' list and shown joining "(computer)". It goes for
  the nearest ship (people a little before other pilots) or an asteroid
  about to hit it, and uses the player's own controls: turn, thrust, brake,
  guns, now and then a small missile, rarely a missile. Each aims a little
  off, by an amount that changes, and pulls the trigger more or less often,
  by its skill. A pilot sent out of the game jumps out and a fresh one
  takes its place. `matanza-ai` is not shipped.
- **Any window size, safely.** A window size is whatever the other end
  says, and the screens are laid out for 80x24, so the server keeps its
  idea of a window between 80x24 and 400x200, and text that would fall
  outside the window is left out instead of written past the buffer (a
  crash on a small window). Ctrl-L redraws the whole screen.
- **Fixes found under AddressSanitizer and UBSan.** A player who left
  during the opening animation or the menu had their connection freed
  while the animation went on drawing into it (use after free): a dropped
  connection now draws nothing, and its buffers are cleared when freed. A
  send that went out in part dropped the rest of the frame, leaving the
  terminal inside an escape sequence; the rest is now kept and sent first,
  with a full redraw after, and a player who stops reading for a megabyte
  is let go. Bullets seen through another ship (`[` and `]`) were scaled
  to that ship's window and, when it was bigger than yours, drawn past the
  end of your screen buffer; no point outside the screen is drawn now. The
  image dither's random numbers overflowed a signed int. A player who left
  before the telnet negotiation finished had animation data freed that was
  never set; connections now start zeroed.
  Connection buffers were leaked when a connection was removed.
- **One Enter on the name.** With no password and no teams (always the
  case here), Enter on the name moved the focus to an unseen fourth field
  and the game only started on a second Enter, so the screen looked stuck.
  Enter on the name now starts the game; an empty name is still refused.
- **Builds with today's compilers and C libraries.** `time.h` for `time()`,
  `string.h` in the bundled getopt (musl and macOS use it), and
  `-std=gnu99` for the 2000-era declarations.

## The launcher (`launcher/`, Rust)

New: `matanza`, one program that plays and serves, in the shape of our
SSHTron. With no options it opens a menu:

- **Play against computer ships**: starts the server with N computer
  ships (default 3, Left and Right change it, up to 8) and connects this
  terminal straight to its socket. No network at all.
- **Host a game on your network**: the same, plus an SSH server (russh
  0.65, ring) on port 2022 or the next free one up to 2031. The screen
  shows the addresses to join with. Anyone on the local network may join
  without a password (the game asks for a name); connections from outside
  private, link-local and loopback ranges are refused. Each SSH session
  with a terminal gets its own connection to the game's socket; window
  changes become telnet NAWS. When the host leaves, the game ends for
  everyone.
- **Join a friend**: a built-in SSH client (russh), so no `ssh` is needed.
  A server's key is remembered on first use in `known_hosts` and a changed
  key is refused. Plain `ssh -p 2022 host` works too.

Options: `--play`, `--host`, `--join HOST[:PORT]`, `--server` (host without
playing), `--ships N`, `--port N`. The host key (ed25519, made on first
run), `known_hosts`, `settings` (computer ships, last address joined),
`matanza.log` and `server.log` live in `$XDG_STATE_HOME/matanza`
(`~/.local/state/matanza`; `~/Library/Application Support/matanza` on
macOS). The game's socket is there too, or in a private folder under
`$XDG_RUNTIME_DIR` (or `/tmp`) when that path is too long for a socket.
`RUST_LOG=debug` logs every key and window change. The server runs beside
the launcher as `matanza-server` and is stopped with it.

The launcher answers the server's telnet: terminal type `xterm`, window
size by NAWS (from the pty request, window-change requests or SIGWINCH),
IAC escaped both ways. The game draws plain ASCII with a few colours (red,
blue, bold, reverse); anything above 127 would be shown as a space.

## Packaging

- `asciiverse/build.sh`: tangles and builds the server (without libpng:
  PNG backgrounds are an option of the server's own) and builds the
  launcher, into `build/$PLATFORM/`.
- `asciiverse/package.sh`: `dist/matanza-<target>.tar.gz` with
  `matanza`, `matanza-server`, `LICENSE` (COPYING), `README.md` and this
  file.
- `.github/workflows/release.yml`: on our tags (`*-av.*`), static Linux
  (x86-64 and ARM64, musl) and macOS (x86-64 and ARM64), plus SHA256SUMS.
  No Windows: the server listens only on a Unix domain socket and the
  launcher's terminal handling is Unix only.
- `asciiverse/record-preview.sh`: the catalog preview, 100x30.

## Checked

Linux, glibc and static musl. Local play against computer ships in tmux at
100x30 and resized to 120x40; hosting with the built-in client and OpenSSH
joining at the same time, at 80x24, 70x20 and 60x20, computer ships making
way as each joined, and every client let go when the host quit.

The server under AddressSanitizer and UBSan, with eight computer ships and
six bots that join, play every key (arrows, guns, missiles, views, zoom,
pause, chat, Ctrl-C, a raw 255), send window sizes from 1x1 to 400x120,
and leave by `q` or by dropping the connection, some before the telnet
negotiation ends. Three soaks found the use after free, the bad free and
the drawing overflow above; after their fixes, a 30-minute soak ran clean:
189 sessions, 76 MB drawn, no reports, and no leaks at exit.

0.15 was built for comparison (with gnu89 inline semantics, `-fcommon` and
the png fix) but could not be played: it needs a universe written in its
Scheme, and the release ships none.
