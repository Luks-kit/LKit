#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TOY=$ROOT/build/toy
if [ ! -x "$TOY" ]; then
  echo "binary not found; building..."
  make -C "$ROOT"
fi

# Test 1: basic decl + assign
OUT=$({ printf "let x = (2);\n x = 3;\n" | "$TOY"; } | grep "^Result:" | tail -n1 || true)
if [[ "$OUT" != "Result: 3" ]]; then
  echo "Test1 failed: expected 'Result: 3' got: $OUT"
  exit 2
fi

echo "All tests passed"
