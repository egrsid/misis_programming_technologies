mod json_io;
use json_io::Cursor;
use std::io::Read;

fn solve(initial: i64, ops: &str, ids: &[i64], counts: &[i64]) -> (String, Vec<[i64; 3]>) {
    // TODO: Реализуйте Warehouse и его методы; состояние меняется только внутри объекта.
    let _ = (initial, ids, counts);
    ("?".repeat(ops.len()), Vec::new())
}

fn main() {
    let mut raw = Vec::new();
    std::io::stdin().read_to_end(&mut raw).unwrap();
    let initial = Cursor::at(&raw, "initial").num();
    let ops = Cursor::at(&raw, "ops").str();
    let mut ids = Vec::new();
    Cursor::at(&raw, "ids").arr(|c| ids.push(c.num()));
    let mut counts = Vec::new();
    Cursor::at(&raw, "counts").arr(|c| counts.push(c.num()));
    let (results, snapshots) = solve(initial, &ops, &ids, &counts);
    println!("{{\"results\":\"{results}\",\"snapshots\":{snapshots:?}}}");
}
