#include "day.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

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
  long long corners[line_count][2] = {};
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
      corners[l][n] = acc;
      c++;
    }
  }
  for (size_t a = 0; a < line_count; a++) {
    for (size_t b = 0; b < line_count; b++) {
      long long minx = MIN(corners[a][0], corners[b][0]);
      long long miny = MIN(corners[a][1], corners[b][1]);
      long long maxx = MAX(corners[a][0], corners[b][0]);
      long long maxy = MAX(corners[a][1], corners[b][1]);
      long long w = maxx - minx;
      long long h = maxy - miny;
      long long area = (w + 1) * (h + 1);
      if (area > largest) {
        if (!convex) {
          bool overlap = false;
          for (size_t c = 0; c < line_count; c++) {
            size_t cc = (c + 1) % line_count;
            long long x1 = MIN(corners[c][0], corners[cc][0]);
            long long y1 = MIN(corners[c][1], corners[cc][1]);
            long long x2 = MAX(corners[c][0], corners[cc][0]);
            long long y2 = MAX(corners[c][1], corners[cc][1]);
            overlap = overlap ||
                      !(x2 <= minx || maxx <= x1 || y2 <= miny || maxy <= y1);
          }
          if (overlap) {
            continue;
          }
        }
        largest = area;
      }
    }
  }
  return largest;
}
