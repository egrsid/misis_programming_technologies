#!/usr/bin/env sh
# Локальная самопроверка решения против открытых примеров (examples.ndjson).
#
# Это НЕ оценка: закрытые тесты преподавателя студент не видит. Скрипт лишь
# прогоняет точку входа выбранного языка ровно так, как это сделает Runner
# (одна JSON-строка в stdin → одна JSON-строка в stdout), и сравнивает вывод
# с `expected` из примеров. Помогает поймать ошибки контракта до отправки MR.
#
# Использование:
#   ./selfcheck.sh python     # или go | rust | cpp | zig
#
# Зависимости: только python3 (используется для разбора JSON примеров —
# не для решения). Решение запускается родной командой своего языка.
set -eu

LANG="${1:-}"
DIR="$(cd "$(dirname "$0")" && pwd)"
EXAMPLES="$DIR/examples.ndjson"
# Своя папка сборки на каждый каталог: иначе CMake подхватит чужой кэш.
BUILD="${TMPDIR:-/tmp}/selfcheck-$(printf %s "$DIR" | cksum | cut -d" " -f1)"

case "$LANG" in
  go)     RUN() { ( cd "$DIR/go"   && go run . ); } ;;
  python) RUN() { ( cd "$DIR/python" && python3 main.py ); } ;;
  rust)   RUN() { ( cd "$DIR/rust" && cargo run --locked --quiet ); } ;;
  cpp)    RUN() {
            cmake -S "$DIR/cpp" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release >/dev/null 2>&1
            cmake --build "$BUILD" --target solution >/dev/null 2>&1
            "$BUILD"/solution
          } ;;
  zig)    RUN() { ( cd "$DIR/zig" && zig build run 2>/dev/null ); } ;;
  *)
    echo "usage: $0 <go|python|rust|cpp|zig>" >&2
    exit 2 ;;
esac

[ -f "$EXAMPLES" ] || { echo "нет файла examples.ndjson рядом со скриптом" >&2; exit 2; }
command -v python3 >/dev/null 2>&1 || { echo "нужен python3 для разбора примеров" >&2; exit 2; }

pass=0; fail=0
while IFS= read -r line; do
  [ -n "$line" ] || continue
  id=$(printf '%s' "$line"    | python3 -c 'import sys,json;print(json.load(sys.stdin)["id"])')
  inp=$(printf '%s' "$line"   | python3 -c 'import sys,json;print(json.dumps(json.load(sys.stdin)["input"],ensure_ascii=False))')
  exp=$(printf '%s' "$line"   | python3 -c 'import sys,json;print(json.dumps(json.load(sys.stdin)["expected"],ensure_ascii=False))')

  got=$(printf '%s\n' "$inp" | RUN | tr -d '\r\n' || true)
  # Сравниваем как JSON: "electronics" == "electronics", пробелы/кавычки важны.
  norm_got=$(printf '%s' "$got" | python3 -c 'import sys,json;print(json.dumps(json.loads(sys.stdin.read() or "null"),ensure_ascii=False))' 2>/dev/null || printf '%s' "$got")
  if [ "$norm_got" = "$exp" ]; then
    pass=$((pass+1)); printf '  ok   %-8s → %s\n' "$id" "$got"
  else
    fail=$((fail+1)); printf '  FAIL %-8s → got=%s exp=%s\n' "$id" "$got" "$exp"
  fi
done < "$EXAMPLES"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
