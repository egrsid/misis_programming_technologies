use std::collections::HashSet;
use std::io::Read;

struct Cursor<'a> {
    s: &'a [u8],
    p: usize,
}

impl<'a> Cursor<'a> {
    fn at(s: &'a [u8], key: &str) -> Self {
        let k = format!("\"{key}\"");
        let k = k.as_bytes();
        let mut from = 0;

        while let Some(off) = s[from..].windows(k.len()).position(|w| w == k) {
            let mut c = Cursor {
                s,
                p: from + off + k.len(),
            };

            if c.eat(b':') {
                return c;
            }

            from += off + 1;
        }

        Cursor { s, p: s.len() }
    }

    fn ws(&mut self) {
        while self.p < self.s.len() && self.s[self.p].is_ascii_whitespace() {
            self.p += 1;
        }
    }

    fn eat(&mut self, c: u8) -> bool {
        self.ws();

        if self.p < self.s.len() && self.s[self.p] == c {
            self.p += 1;
            true
        } else {
            false
        }
    }

    fn str(&mut self) -> String {
        self.ws();
        self.p += 1;
        let b = self.p;

        while self.s[self.p] != b'"' {
            self.p += 1;
        }

        self.p += 1;
        String::from_utf8(self.s[b..self.p - 1].to_vec()).unwrap()
    }

    fn arr(&mut self, mut f: impl FnMut(&mut Self)) {
        self.eat(b'[');

        if self.eat(b']') {
            return;
        }

        loop {
            f(self);

            if !self.eat(b',') {
                break;
            }
        }

        self.eat(b']');
    }
}

type Op = Vec<String>;

// TODO: run ops, return the event log ending with "result ...".
// Use a Drop-based scope, Result and a trait.
fn solve(fo: HashSet<String>, fc: HashSet<String>, fw: HashSet<String>, ops: &[Op]) -> Vec<String> {
    let _ = (fo, fc, fw, ops);
    Vec::new()
}

fn main() {
    let mut buf = Vec::new();
    std::io::stdin().read_to_end(&mut buf).unwrap();
    let mut sets: Vec<HashSet<String>> = Vec::new();

    for key in ["fail_open", "fail_close", "fail_work"] {
        let mut s = HashSet::new();
        Cursor::at(&buf, key).arr(|c| {
            s.insert(c.str());
        });
        sets.push(s);
    }

    let mut ops = Vec::new();
    Cursor::at(&buf, "ops").arr(|c| {
        let mut op = Vec::new();
        c.arr(|c| op.push(c.str()));
        ops.push(op);
    });
    let fw = sets.pop().unwrap();
    let fc = sets.pop().unwrap();
    let fo = sets.pop().unwrap();
    let res = solve(fo, fc, fw, &ops);
    print!(
        "[{}]",
        res.iter()
            .map(|s| format!("\"{s}\""))
            .collect::<Vec<_>>()
            .join(",")
    );
}
