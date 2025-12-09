#include "day.h"
#include <stdbool.h>
#include <stdio.h>

#define DIMS 2

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

long long calc(size_t size, char *buffer, bool convex);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }

long long calc(size_t size, char *buffer, bool convex) {
  long long largest_area = 0;

  size_t line_count = 1;
  {
    // count total lines
    int pos = 0;
    while (buffer[pos] != '\0' && pos <= (int)size) {
      if (buffer[pos] == '\n') {
        line_count++;
      }
      pos++;
    }
  }
  //
  long long vertices[line_count][DIMS] = {};
  {
    int c = 0;
    for (size_t l = 0; l < line_count; l++) {
      for (size_t n = 0; n < DIMS; n++) {
        long long acc = 0;
        while (buffer[c] != '\0' && c <= (int)size && buffer[c] != '\n' &&
               buffer[c] != ',') {
          int d = buffer[c] - '0';
          acc = acc * 10 + d;
          c++;
        }
        vertices[l][n] = acc;
        c++;
      }
    }
  }
  for (size_t a = 0; a < line_count; a++) {
    for (size_t b = 0; b < line_count; b++) {
      long long minx = MIN(vertices[a][0], vertices[b][0]);
      long long miny = MIN(vertices[a][1], vertices[b][1]);
      long long maxx = MAX(vertices[a][0], vertices[b][0]);
      long long maxy = MAX(vertices[a][1], vertices[b][1]);
      long long w = maxx - minx;
      long long h = maxy - miny;
      long long area = (w + 1) * (h + 1);

      if (area > largest_area) {
        // for part 2 check convexity
        if (convex) {
          bool overlap = false;
          for (size_t c = 0; c < line_count && !overlap; c++) {
            size_t cc = (c + 1) % line_count;

            long long x1 = MIN(vertices[c][0], vertices[cc][0]);
            long long y1 = MIN(vertices[c][1], vertices[cc][1]);
            long long x2 = MAX(vertices[c][0], vertices[cc][0]);
            long long y2 = MAX(vertices[c][1], vertices[cc][1]);

            overlap = overlap ||
                      !(x2 <= minx || maxx <= x1 || y2 <= miny || maxy <= y1);
          }
          if (overlap) {
            continue;
          }
        }
        largest_area = area;
      }
    }
  }

  return largest_area;
}
