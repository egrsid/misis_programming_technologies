mod json_io;
use json_io::Cursor;
use std::io::Read;

fn solve(prices: &[i64], codes: &str, amounts: &[i64], thresholds: &[i64]) -> Vec<i64> {
    // TODO: Создайте три типа Discount с apply и общий конвейер без ветвления по типам.
    let _ = (prices, codes, amounts, thresholds);
    Vec::new()
}

fn main() {
    let mut raw = Vec::new();
    std::io::stdin().read_to_end(&mut raw).unwrap();
    let rules = Cursor::at(&raw, "rules").str();
    let read = |key| {
        let mut values = Vec::new();
        Cursor::at(&raw, key).arr(|c| values.push(c.num()));
        values
    };
    println!(
        "{:?}",
        solve(
            &read("prices"),
            &rules,
            &read("amounts"),
            &read("thresholds")
        )
    );
}
