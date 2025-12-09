#!/usr/bin/env bash
set -e
RUNONLY=false
NOOUTPUT=false
RUNTEST=true
RUNPROD=true

# Parse options
while getopts ":rstp" opt; do
  case "$opt" in
    r) RUNONLY=true ;;
    s) NOOUTPUT=true ;;
    t) RUNPROD=false ;;
    p) RUNTEST=false ;;
    *)  [ -z "$NOOUTPUT" ] && echo "Usage: $0 [-r] [-s] [days...]" >&2
       exit 1 ;;
  esac
done

# Remove options from the positional parameters
shift $((OPTIND - 1))
if (( $# >= 1 )); then
  DAYS="$@"
  [ -z "$NOOUTPUT" ] &&  echo "Selected Days: ${DAYS}"
else
  ALL_DAYS=(inputs/*)
  ALL_DAYS=("${ALL_DAYS[@]##*/}")
  DAYS="${ALL_DAYS[@]}"

  [ -z "$NOOUTPUT" ] &&  echo "All Days: ${DAYS}"
fi

for DAY in $DAYS
do
mkdir -p build
if [ -f "$DAY.c" ]; then
if [ "$RUNONLY" = false ]; then
  [ -z "$NOOUTPUT" ] &&  echo "Compiling $DAY:"
  [ -z "$NOOUTPUT" ] &&  echo ""
  gcc -O3 -Werror -Wall -Wextra -Wuninitialized -D DAY=$DAY main.c -o build/$DAY
fi
 [ -z "$NOOUTPUT" ] && echo "Running $DAY:"
 [ "$RUNTEST" = true ] && ./build/$DAY ./inputs/$DAY/example.txt
 [ "$RUNPROD" = true ] && ./build/$DAY ./inputs/$DAY/prod.txt
fi
done
