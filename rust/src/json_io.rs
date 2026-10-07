// ---------- Минимальный разбор JSON под формат задачи ----------
pub struct Cursor<'a> {
    s: &'a [u8],
    p: usize,
}

impl<'a> Cursor<'a> {
    /// Курсор сразу после `"key":`.
    pub fn at(s: &'a [u8], key: &str) -> Self {
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
    pub fn ws(&mut self) {
        while self.p < self.s.len() && self.s[self.p].is_ascii_whitespace() {
            self.p += 1;
        }
    }
    pub fn eat(&mut self, c: u8) -> bool {
        self.ws();
        if self.p < self.s.len() && self.s[self.p] == c {
            self.p += 1;
            true
        } else {
            false
        }
    }
    pub fn num(&mut self) -> i64 {
        self.ws();
        let neg = self.eat(b'-');
        let mut v: i64 = 0;
        while self.p < self.s.len() && self.s[self.p].is_ascii_digit() {
            v = v * 10 + (self.s[self.p] - b'0') as i64;
            self.p += 1;
        }
        if neg { -v } else { v }
    }
    pub fn str(&mut self) -> String {
        self.ws();
        self.eat(b'"');
        let from = self.p;
        while self.p < self.s.len() && self.s[self.p] != b'"' {
            self.p += 1;
        }
        self.p += 1;
        String::from_utf8_lossy(&self.s[from..self.p - 1]).into_owned()
    }
    pub fn arr(&mut self, mut f: impl FnMut(&mut Self)) {
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
