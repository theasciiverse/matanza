//! The telnet side of the game server.
//!
//! The server speaks telnet on its Unix socket: it asks for the terminal type
//! and the window size (NAWS) before it starts, and it may send negotiations
//! at any time. A `Telnet` sits between one player (the local terminal or an
//! SSH session) and that socket: it answers the negotiations, strips them
//! from what reaches the player, escapes the player's bytes, and turns window
//! sizes into NAWS.

const IAC: u8 = 255;
const DONT: u8 = 254;
const DO: u8 = 253;
const WONT: u8 = 252;
const WILL: u8 = 251;
const SB: u8 = 250;
const SE: u8 = 240;

const OPT_ECHO: u8 = 1;
const OPT_SGA: u8 = 3;
const OPT_TTYPE: u8 = 24;
const OPT_NAWS: u8 = 31;

const TTYPE_IS: u8 = 0;
const TTYPE_SEND: u8 = 1;

#[derive(Clone, Copy, Debug, PartialEq)]
enum State {
    Data,
    Iac,
    Option(u8),
    Sub,
    SubIac,
}

pub struct Telnet {
    state: State,
    sub: Vec<u8>,
    cols: u16,
    rows: u16,
    naws_on: bool,
}

impl Telnet {
    pub fn new(cols: u16, rows: u16) -> Self {
        Telnet {
            state: State::Data,
            sub: Vec::new(),
            cols: cols.max(1),
            rows: rows.max(1),
            naws_on: false,
        }
    }

    /// Bytes from the game: what the player should see goes to `screen`,
    /// answers for the game go to `reply`.
    pub fn from_game(&mut self, input: &[u8], screen: &mut Vec<u8>, reply: &mut Vec<u8>) {
        for &b in input {
            match self.state {
                State::Data => {
                    if b == IAC {
                        self.state = State::Iac;
                    } else {
                        screen.push(safe(b));
                    }
                }
                State::Iac => match b {
                    IAC => {
                        screen.push(safe(IAC));
                        self.state = State::Data;
                    }
                    DO | DONT | WILL | WONT => self.state = State::Option(b),
                    SB => {
                        self.sub.clear();
                        self.state = State::Sub;
                    }
                    _ => self.state = State::Data,
                },
                State::Option(cmd) => {
                    self.negotiate(cmd, b, reply);
                    self.state = State::Data;
                }
                State::Sub => {
                    if b == IAC {
                        self.state = State::SubIac;
                    } else if self.sub.len() < 256 {
                        self.sub.push(b);
                    }
                }
                State::SubIac => {
                    if b == SE {
                        self.subnegotiation(reply);
                        self.state = State::Data;
                    } else {
                        if self.sub.len() < 256 {
                            self.sub.push(b);
                        }
                        self.state = State::Sub;
                    }
                }
            }
        }
    }

    fn negotiate(&mut self, cmd: u8, opt: u8, reply: &mut Vec<u8>) {
        match (cmd, opt) {
            (DO, OPT_NAWS) => {
                reply.extend_from_slice(&[IAC, WILL, OPT_NAWS]);
                self.naws_on = true;
                reply.extend_from_slice(&self.naws());
            }
            (DO, OPT_TTYPE) => reply.extend_from_slice(&[IAC, WILL, OPT_TTYPE]),
            (DO, o) => reply.extend_from_slice(&[IAC, WONT, o]),
            (WILL, OPT_ECHO) | (WILL, OPT_SGA) => reply.extend_from_slice(&[IAC, DO, opt]),
            (WILL, o) => reply.extend_from_slice(&[IAC, DONT, o]),
            // DONT and WONT need no answer from us.
            _ => {}
        }
    }

    fn subnegotiation(&mut self, reply: &mut Vec<u8>) {
        if self.sub.len() >= 2 && self.sub[0] == OPT_TTYPE && self.sub[1] == TTYPE_SEND {
            reply.extend_from_slice(&[IAC, SB, OPT_TTYPE, TTYPE_IS]);
            reply.extend_from_slice(b"xterm");
            reply.extend_from_slice(&[IAC, SE]);
        }
    }

    /// A new window size; returns the NAWS message for the game, if the game
    /// has asked for sizes yet (it gets the size when it asks otherwise).
    pub fn resize(&mut self, cols: u16, rows: u16) -> Option<Vec<u8>> {
        self.cols = cols.max(1);
        self.rows = rows.max(1);
        if self.naws_on {
            Some(self.naws())
        } else {
            None
        }
    }

    fn naws(&self) -> Vec<u8> {
        let mut v = vec![IAC, SB, OPT_NAWS];
        for x in [self.cols >> 8, self.cols & 0xff, self.rows >> 8, self.rows & 0xff] {
            let x = x as u8;
            v.push(x);
            if x == IAC {
                v.push(IAC);
            }
        }
        v.extend_from_slice(&[IAC, SE]);
        v
    }
}

/// Keys from the player, escaped for telnet.
pub fn to_game(input: &[u8], out: &mut Vec<u8>) {
    for &b in input {
        out.push(b);
        if b == IAC {
            out.push(IAC);
        }
    }
}

/// The game draws in plain ASCII; anything else would not be valid UTF-8 on
/// the player's terminal, so it is shown as a space.
fn safe(b: u8) -> u8 {
    if b < 0x80 {
        b
    } else {
        b' '
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn answers_the_opening_negotiation() {
        let mut t = Telnet::new(100, 30);
        let mut screen = Vec::new();
        let mut reply = Vec::new();
        let opening = [
            b"hi ".as_slice(),
            &[IAC, DO, OPT_TTYPE, IAC, DO, OPT_NAWS, IAC, DONT, 34, IAC, WILL, OPT_SGA, IAC, WILL, OPT_ECHO],
            b"there",
        ]
        .concat();
        t.from_game(&opening, &mut screen, &mut reply);
        assert_eq!(screen, b"hi there");
        assert_eq!(
            reply,
            [
                IAC, WILL, OPT_TTYPE, IAC, WILL, OPT_NAWS, IAC, SB, OPT_NAWS, 0, 100, 0, 30, IAC, SE, IAC, DO,
                OPT_SGA, IAC, DO, OPT_ECHO
            ]
        );
    }

    #[test]
    fn sends_the_terminal_type_when_asked() {
        let mut t = Telnet::new(80, 24);
        let (mut screen, mut reply) = (Vec::new(), Vec::new());
        // split across two reads
        t.from_game(&[IAC, SB, OPT_TTYPE], &mut screen, &mut reply);
        t.from_game(&[TTYPE_SEND, IAC, SE, b'x'], &mut screen, &mut reply);
        assert_eq!(screen, b"x");
        assert_eq!(reply, [&[IAC, SB, OPT_TTYPE, TTYPE_IS][..], b"xterm", &[IAC, SE]].concat());
    }

    #[test]
    fn resizes_after_naws_is_on() {
        let mut t = Telnet::new(80, 24);
        assert_eq!(t.resize(90, 30), None);
        let (mut screen, mut reply) = (Vec::new(), Vec::new());
        t.from_game(&[IAC, DO, OPT_NAWS], &mut screen, &mut reply);
        assert_eq!(t.resize(255, 300), Some(vec![IAC, SB, OPT_NAWS, 0, 255, 255, 1, 44, IAC, SE]));
    }

    #[test]
    fn escapes_and_unescapes_iac() {
        let mut out = Vec::new();
        to_game(&[b'a', IAC, b'b'], &mut out);
        assert_eq!(out, [b'a', IAC, IAC, b'b']);
        let mut t = Telnet::new(80, 24);
        let (mut screen, mut reply) = (Vec::new(), Vec::new());
        t.from_game(&[b'a', IAC, IAC, 0xe9, b'b'], &mut screen, &mut reply);
        assert_eq!(screen, b"a  b");
    }
}
