# Matanza

**Your ship is a smear of ASCII art in the middle of the screen, the radar
at the edge says `V` is coming in fast, and in a moment the space between
you fills with bullets.**

Matanza is Alejandro Forero Cuervo's multiplayer space battle, "the
multiplayer, action, network-based, real time, ASCII art based, spaceship,
slogan-lacking game" (and it is pronounced *Matanga*). Every ship, asteroid
and missile is a real image, rotated through 64 angles and drawn into
characters on the fly, so the picture gets finer as your window gets bigger.
Ships drift as ships do in space: turn, thrust, coast, brake. Letters at the
edge of the screen point at the other ships, lowercase when they are far,
capitals when they are close, and an asterisk when it is time to shoot.

This is the Asciiverse edition. It plays in one terminal against computer
ships, and friends on the same network join over SSH: no telnet, no server
to set up.

## Playing

Run `matanza` and choose:

- **Play against computer ships.** A battle on this machine, nothing on the
  network. Three computer ships fly against you (Left and Right on the menu
  change how many, up to eight).
- **Host a game on your network.** The same battle, open to the local
  network on port 2022 (or the next free one). The screen shows the address
  friends join with. Each friend who joins takes the place of a computer
  ship.
- **Join a friend.** Type the address their screen shows. A friend can also
  join with plain `ssh -p 2022 <address>`.

In the game, press Enter through the opening, type a name of up to eight
letters (one is offered) and press Enter again.

| Key | |
|---|---|
| Left, Right (or `h`, `l`) | turn |
| Up (or `k`) | thrust |
| Down (or `j`) | brake |
| Space | fire; `b` fires backwards |
| `n` | small missile (seven) |
| `m` | missile (two) |
| `+`, `-` | zoom in and out |
| `[`, `]` | watch the battle from another ship |
| Enter | say something to everyone |
| `w`, `z` | who is playing, and the scores |
| `c`, `C`, `s`, `v` | drop a message, clear them, move them, hide them |
| `p` | pause the universe (for everyone) |
| Ctrl-L | draw the screen again |
| `q` | quit |

Every ship has five lives. Lose them and you are sent out of the game.

## Options

```
matanza --play              play here against computer ships
matanza --host              play here, open to the local network
matanza --join HOST[:PORT]  join a friend
matanza --server            host without playing
matanza --ships N           computer ships (default 3, up to 8)
matanza --port N            the port to host on
```

The host key, the servers you have joined, settings and logs live in
`$XDG_STATE_HOME/matanza` (`~/.local/state/matanza`). `RUST_LOG=debug`
writes every key and window change to `matanza.log` there; the game
server's own messages go to `server.log`.

## History

Alejandro Forero Cuervo started Matanza in 2000 as part of Freaks Unidos,
the group behind bachue.com, and wrote it as a literate program in CWEB. The
first version came out on 4 September 2000, and the game lived on telnet:
you ran the server, everyone else typed `telnet yourhost 7993`, and the
server worked out each player's terminal size and drew the battle for it.
His README promised computer-controlled ships one day, and a machine
protocol, MMP, so programs could fly them; `matanza-ai` was its first step.
Version 0.13 is the one Debian has carried since 2004, kept building by its
games team ever since. bachue.com is now only in the Wayback Machine.

This edition takes Debian's patches as they are, then puts SSH in front of
the server instead of telnet, and finally gives the game the computer ships
it was waiting for. What changed, and why, is in [PATCHES.md](PATCHES.md).

## Building

Needs `ctangle` (CWEB, in TeX Live), a C compiler and Rust:

```
sh asciiverse/build.sh
```

The launcher (`launcher/`, Rust) and the game server (`matanza.w`, C) land
in `build/linux/`; the launcher finds `matanza-server` beside itself.

## Credits

Matanza by Alejandro Forero Cuervo (Freaks Unidos), 2000 to 2003, GPL-2.0 or
later. BeOS testing by Andrés Felipe Arguello. Expat by the Thai Open Source
Software Center; getopt by the Free Software Foundation. Debian packaging and
patches by Polkan Alexis Garcia Rodriguez, Frank Lichtenheld, Peter
Pentchev, Bart Martens, Nobuhiro Iwamatsu, Boris Pek, Patrice Duroux,
Alexandre Detiste and the Debian Games Team. The original README follows in
`README`.
