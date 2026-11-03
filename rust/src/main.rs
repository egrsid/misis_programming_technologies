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

    fn num(&mut self) -> usize {
        self.ws();
        let mut v = 0;

        while self.p < self.s.len() && self.s[self.p].is_ascii_digit() {
            v = v * 10 + (self.s[self.p] - b'0') as usize;
            self.p += 1;
        }

        v
    }

    fn str(&mut self) -> &'a str {
        self.ws();
        self.p += 1;
        let b = self.p;

        while self.s[self.p] != b'"' {
            self.p += 1;
        }

        self.p += 1;
        std::str::from_utf8(&self.s[b..self.p - 1]).unwrap()
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

struct Op {
    code: u8,
    a: usize,
    b: usize,
    c: usize,
}

// TODO: run ops, return the event log ("d<id>", "u<n>", "leak<id>").
// Use Rc/Weak and Drop; free fields without recursion.
fn solve(nvars: usize, ops: &[Op]) -> Vec<String> {
    let _ = (nvars, ops.iter().map(|o| (o.code, o.a, o.b, o.c)).count());
    Vec::new()
}

fn main() {
    let mut buf = Vec::new();
    std::io::stdin().read_to_end(&mut buf).unwrap();
    let nvars = Cursor::at(&buf, "vars").num();
    let mut ops = Vec::new();
    Cursor::at(&buf, "ops").arr(|c| {
        let mut o = Op {
            code: 0,
            a: 0,
            b: 0,
            c: 0,
        };

        let mut i = 0;
        c.arr(|c| {
            match i {
                0 => o.code = c.str().as_bytes()[0],
                1 => o.a = c.num(),
                2 => o.b = c.num(),
                _ => o.c = c.num(),
            }

            i += 1;
        });
        ops.push(o);
    });
    let res = solve(nvars, &ops);
    print!(
        "[{}]",
        res.iter()
            .map(|s| format!("\"{s}\""))
            .collect::<Vec<_>>()
            .join(",")
    );
}
