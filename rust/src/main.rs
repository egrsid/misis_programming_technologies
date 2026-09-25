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
/// TODO: для каждого запроса вернуть 'A' (пропустить) или 'D' (отклонить).
/// Нужна амортизированная O(1) на запрос при фиксированном числе правил.
fn solve(rules: &[(i64, i64)], users: &[i64], times: &[i64]) -> String {
    let _ = (rules, times);
    "A".repeat(users.len())
}

fn main() {
    let mut buf = Vec::new();
    std::io::stdin().read_to_end(&mut buf).unwrap();
    let mut rules = Vec::new();
    let mut c = Cursor::at(&buf, "rules");
    c.arr(|c| {
        let mut pair = Vec::with_capacity(2);
        c.arr(|c| pair.push(c.num()));
        rules.push((pair[0], pair[1]));
    });
    let mut users = Vec::new();
    Cursor::at(&buf, "users").arr(|c| users.push(c.num()));
    let mut times = Vec::new();
    Cursor::at(&buf, "times").arr(|c| times.push(c.num()));
    print!("\"{}\"", solve(&rules, &users, &times));
}
