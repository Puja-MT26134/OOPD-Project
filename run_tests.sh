#!/bin/sh
# Group ID: 79
# Runs every tests/*.in through ./acaderp and diffs stdout against the .out file.
pass=0
total=0
for f in tests/*.in; do
    base=${f%.in}
    total=$((total + 1))
    if ./acaderp < "$f" 2>/dev/null | diff -q - "$base.out" >/dev/null; then
        pass=$((pass + 1))
    else
        echo "FAIL: $base"
    fi
done
echo "Passed $pass / $total"
[ "$pass" -eq "$total" ]
