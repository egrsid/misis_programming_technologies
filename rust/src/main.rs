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
}

// ---------- Генератор splitmix64 (общий для всех языков) ----------
struct Rng(u64);

impl Rng {
    fn next(&mut self) -> u64 {
        self.0 = self.0.wrapping_add(0x9E3779B97F4A7C15);
        let mut z = self.0;
        z = (z ^ (z >> 30)).wrapping_mul(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)).wrapping_mul(0x94D049BB133111EB);
        z ^ (z >> 31)
    }
    fn rnd(&mut self) -> u64 {
        self.next() % 1_000_000
    }
}

// ---------- Решение ----------
/// TODO: container ∈ {array, linked, deque} → Vec / LinkedList / VecDeque,
/// workload ∈ {push_back, push_front, queue, sorted_insert} — см. README.
/// Курсоры LinkedList нестабильны: вставка в середину — через split_off + append.
/// checksum = Σ (i + 1) · a[i] mod 1 000 000 007.
#[allow(unused_variables, unused_mut)]
fn run(container: &str, workload: &str, n: u64, seed: u64) -> (usize, u64) {
    let mut r = Rng(seed);
    (n as usize, 0)
}

fn main() {
    let mut buf = Vec::new();
    std::io::stdin().read_to_end(&mut buf).unwrap();
    let container = Cursor::at(&buf, "container").str();
    let workload = Cursor::at(&buf, "workload").str();
    let n = Cursor::at(&buf, "n").num() as u64;
    let seed = Cursor::at(&buf, "seed").num() as u64;
    let (len, sum) = run(&container, &workload, n, seed);
    print!("{{\"len\": {len}, \"checksum\": {sum}}}");
}
