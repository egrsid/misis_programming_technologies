mod json_io;
use json_io::Cursor;
use std::io::Read;

struct Resources {
    cpu: i64,
    ram: i64,
    disk: i64,
}

fn solve(left: &[Resources], right: &[Resources]) -> String {
    // TODO: Реализуйте Resources, сложение и частичный порядок; unordered не равен equal.
    let _ = (left, right);
    "[]".to_owned()
}

fn main() {
    let mut raw = Vec::new();
    std::io::stdin().read_to_end(&mut raw).unwrap();
    let read = |key| {
        let mut values = Vec::new();
        Cursor::at(&raw, key).arr(|c| {
            let mut v = Vec::new();
            c.arr(|c| v.push(c.num()));
            values.push(Resources {
                cpu: v[0],
                ram: v[1],
                disk: v[2],
            });
        });
        values
    };
    println!("{}", solve(&read("left"), &read("right")));
}
