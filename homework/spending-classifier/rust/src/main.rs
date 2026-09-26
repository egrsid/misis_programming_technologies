use std::io::{self, Read, Write};

/// Достаёт строковое поле `"key": "..."` из одной JSON-строки, терпимо к
/// пробелам после `:` (описание без экранированных кавычек).
fn field_str(line: &str, key: &str) -> String {
    let k = format!("\"{key}\"");
    let Some(kpos) = line.find(&k) else { return String::new() };
    let after = &line[kpos + k.len()..];
    let Some(colon) = after.find(':') else { return String::new() };
    let Some(q1) = after[colon + 1..].find('"') else { return String::new() };
    let rest = &after[colon + 1 + q1 + 1..];
    match rest.find('"') {
        Some(end) => rest[..end].to_string(),
        None => String::new(),
    }
}

fn classify(_price: f64, _description: &str) -> &'static str {
    // TODO: electronics / groceries / clothing / transport / other.
    "other"
}

fn main() {
    let mut line = String::new();
    io::stdin().read_to_string(&mut line).ok();
    let class = classify(0.0, &field_str(&line, "description"));
    write!(io::stdout(), "\"{class}\"").ok(); // одна JSON-строка
}
