use std::io::Read;

// ---------- Минимальный разбор JSON под формат задачи ----------
struct Cursor<'a> {
    s: &'a [u8],
    p: usize,
}

impl<'a> Cursor<'a> {
    /// Курсор сразу после `"key":`.
    fn at(s: &'a [u8], key: &str) -> Self {
        let k = format!("\"{key}\"");
        let k = k.as_bytes();
        let mut from = 0;
        while let Some(off) = s[from..].windows(k.len()).position(|w| w == k) {
            let mut c = Cursor { s, p: from + off + k.len() };
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
    fn num(&mut self) -> i64 {
        self.ws();
        let neg = self.eat(b'-');
        let mut v: i64 = 0;
        while self.p < self.s.len() && self.s[self.p].is_ascii_digit() {
            v = v * 10 + (self.s[self.p] - b'0') as i64;
            self.p += 1;
        }
        if neg { -v } else { v }
    }
    fn str(&mut self) -> String {
        self.ws();
        self.eat(b'"');
        let from = self.p;
        while self.p < self.s.len() && self.s[self.p] != b'"' {
            self.p += 1;
        }
        self.p += 1;
        String::from_utf8_lossy(&self.s[from..self.p - 1]).into_owned()
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

// ---------- Решение ----------
/// TODO: struct Item с ручными `impl Hash` и `impl PartialEq, Eq` (без `#[derive(Hash)]`),
/// хранение в `HashSet<Item>`. Склеивать товар в строку-ключ нельзя.
/// Название сравнивается без учёта регистра, теги — как множество.
fn solve(ops: &str, names: Vec<String>, tags: Vec<Vec<u32>>) -> String {
    let _ = (names, tags);
    "-".repeat(ops.len())
}

fn main() {
    let mut buf = Vec::new();
    std::io::stdin().read_to_end(&mut buf).unwrap();
    let ops = Cursor::at(&buf, "ops").str();
    let mut names = Vec::new();
    Cursor::at(&buf, "names").arr(|c| names.push(c.str()));
    let mut tags = Vec::new();
    Cursor::at(&buf, "tags").arr(|c| {
        let mut t = Vec::new();
        c.arr(|c| t.push(c.num() as u32));
        tags.push(t);
    });
    print!("\"{}\"", solve(&ops, names, tags));
}
