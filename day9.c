
#include "day.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

long long calc(size_t size, char *buffer, bool convex);

long long part1(size_t size, char *buffer) { return calc(size, buffer, true); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, false); }

long long calc(size_t size, char *buffer, bool convex) {
  long long largest = 0;

  size_t line_count = 1;
  int pos = 0;
  while (buffer[pos] != '\0' && pos <= (int)size &&
         (buffer[pos] != '\n' || buffer[pos + 1] != '\n')) {
    if (buffer[pos] == '\n') {
      line_count++;
    }
    pos++;
  }
  long long tiles[line_count][2] = {};
  int c = 0;
  for (size_t l = 0; l < line_count; l++) {
    for (size_t n = 0; n < 2; n++) {
      long long acc = 0;
      while (buffer[c] != '\0' && c <= (int)size && buffer[c] != '\n' &&
             buffer[c] != ',') {
        int d = buffer[c] - '0';
        acc = acc * 10 + d;
        c++;
      }
      tiles[l][n] = acc;
      c++;
    }
  }
  if (convex) {
    for (size_t a = 0; a < line_count; a++) {
      for (size_t b = 0; b < line_count; b++) {
        long long w = tiles[a][0] - tiles[b][0] + 1;
        long long h = tiles[a][1] - tiles[b][1] + 1;
        long long signed_area = w * h;
        long long area = signed_area < 0 ? -signed_area : signed_area;
        if (area > largest) {
          largest = area;
        }
      }
    }
  } else {
    // TODO part 2
    largest = 0;
  }
  return largest;
}
