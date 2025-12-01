set -e
DAY=$1
echo "Compiling and Running $DAY:"
echo ""
mkdir -p build
gcc -D DAY=$DAY main.c -o build/$DAY
./build/$DAY example
./build/$DAY prod
