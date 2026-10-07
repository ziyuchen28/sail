#!/usr/bin/env bash
set -euo pipefail

run_dir="$PWD/data"
printf '\nRun directory: %s\n\n' "$run_dir"

data_file="$run_dir/sample.bin"

./build/sail "$data_file"

rm -f "$data_file"
