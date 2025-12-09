#include "day.h"
#include <stdbool.h>
#include <stdio.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

long long calc(size_t size, char *buffer);

long long part1(size_t size, char *buffer) { return calc(size, buffer); }
long long part2(size_t size, char *buffer) { return calc(size, buffer); }

long long calc(size_t size, char *buffer) { return buffer[size - 1]; }
