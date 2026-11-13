#!/bin/sh
set -eu

output="$(mktemp)"
binary="${TMPDIR:-/tmp}/gypp-multi-error-test"

if ./gy++ tests/multiple_errors.ycpp -o "$binary" > /dev/null 2>"$output"; then
    echo "FAIL: gy++ accepted a file with syntax errors"
    rm -f "$output" "$binary"
    exit 1
fi

cat "$output"
count="$(grep -c ': error:' "$output" || true)"
rm -f "$output" "$binary"

if [ "$count" -lt 2 ]; then
    echo "FAIL: expected at least two diagnostics, received $count"
    exit 1
fi

echo "PASS: gy++ reported $count independent errors"
