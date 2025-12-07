#!/usr/bin/env bash
set -e

if (( $# > 1 )); then
  DAYS="$@"
else
  ALL_DAYS=(inputs/*)
  ALL_DAYS=("${ALL_DAYS[@]##*/}")
  DAYS="${ALL_DAYS[@]}"
fi

for DAY in $DAYS
do
echo "Compiling and Running $DAY:"
echo ""
mkdir -p build
gcc -O3 -Werror -Wall -Wextra -Wuninitialized -D DAY=$DAY main.c -o build/$DAY
./build/$DAY ./inputs/$DAY/example.txt
./build/$DAY ./inputs/$DAY/prod.txt
done
