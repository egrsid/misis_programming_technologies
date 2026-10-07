#!/usr/bin/env sh
set -eu
if [ "$#" -ne 1 ]; then
    echo "usage: $0 <cpp|rust|zig|go|python>" >&2
    exit 2
fi
solution_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
exec "${PYTHON:-python3}" "$solution_dir/selfcheck.py" "$solution_dir" "$solution_dir/examples.ndjson" "--lang=$1"
