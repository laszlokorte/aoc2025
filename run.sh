#!/usr/bin/env bash
set -e
RUNONLY=false

# Parse options
while getopts "r" opt; do
  case $opt in
    r) RUNONLY=true ;;
    *) echo "Usage: $0 [-r] [days...]" >&2
       exit 1 ;;
  esac
done

# Remove options from the positional parameters
shift $((OPTIND - 1))

if (( $# >= 1 )); then
  DAYS="$@"
  echo "Selected Days: ${DAYS}"
else
  ALL_DAYS=(inputs/*)
  ALL_DAYS=("${ALL_DAYS[@]##*/}")
  DAYS="${ALL_DAYS[@]}"

  echo "All Days: ${DAYS}"
fi

for DAY in $DAYS
do
mkdir -p build
if [ "$RUNONLY" = false ]; then
  echo "Compiling $DAY:"
  echo ""
  gcc -O3 -Werror -Wall -Wextra -Wuninitialized -D DAY=$DAY main.c -o build/$DAY
fi
echo "Running $DAY:"
./build/$DAY ./inputs/$DAY/example.txt
./build/$DAY ./inputs/$DAY/prod.txt
done
