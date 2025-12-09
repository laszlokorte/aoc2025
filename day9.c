#include "day.h"
#include <limits.h>
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
  if (convex) {
    for (size_t a = 0; a < line_count; a++) {
      for (size_t b = 0; b < line_count; b++) {
        long long w = corners[a][0] - corners[b][0] + 1;
        long long h = corners[a][1] - corners[b][1] + 1;
        long long signed_area = w * h;
        long long area = signed_area < 0 ? -signed_area : signed_area;
        if (area > largest) {
          largest = area;
        }
      }
    }
  } else {
    long long minx = INT_MAX;
    long long miny = INT_MAX;
    long long maxx = INT_MIN;
    long long maxy = INT_MIN;
    for (size_t b = 0; b < line_count; b++) {
      long long x = corners[b][0];
      long long y = corners[b][1];
      if (x > maxx)
        maxx = x;
      if (x < minx)
        minx = x;
      if (y > maxy)
        maxy = y;
      if (y < miny)
        miny = y;
    }
    if (line_count > 30) {
      // we know points make up a circle with a thin bar sliced out
      // step 1 find the end points of the bar:

      long long aprox_diam = (maxx - minx);
      long long aprox_rad_squared = aprox_diam * aprox_diam / 4;

      size_t inner_points[2] = {};
      size_t ip = 0;
      for (size_t j = 0; j < line_count; j++) {
        size_t jj = (j + 1) % line_count;
        long long x1 = corners[j][0];
        long long x2 = corners[jj][0];
        long long dx = x2 - x1;
        long long dxsq = dx * dx;
        if (dxsq > aprox_rad_squared / 2) {
          // we found the inner to points
          if (x2 > x1) {
            inner_points[ip++] = jj;
          } else {
            inner_points[ip++] = j;
          }
        }
      }
      for (size_t h = 0; h < ip; h++) {
        long long self_x = corners[inner_points[h]][0];
        long long self_y = corners[inner_points[h]][1];
        long long other_y = corners[inner_points[1 - h]][1];
        long long dir = self_y > other_y ? 1 : -1;
        long long best_y = self_y;
        for (size_t v = 0; v < line_count; v++) {
          if (corners[v][0] > self_x && corners[v][1] * dir > dir * best_y) {
            best_y = corners[v][1];
          }
        }

        for (size_t w = 0; w < line_count; w++) {
          if (corners[w][1] * dir <= dir * best_y &&
              corners[w][1] * dir > dir * self_y) {
            long long check_x = corners[w][0];
            long long check_y = corners[w][1];
            long long dx = check_x - self_x;
            long long dy = check_y - self_y;
            if (dx < 0)
              dx *= -1;
            if (dy < 0)
              dy *= -1;
            long long area = (dx + 1) * (dy + 1);
            if (area > largest) {
              largest = area;
            }
          }
        }
      }
    } else {
      for (size_t a = 0; a < line_count; a++) {
        for (size_t b = 0; b < line_count; b++) {
          long long x1 = corners[a][0];
          long long x2 = corners[b][0];
          long long y1 = corners[a][1];
          long long y2 = corners[b][1];
          if (x1 > x2) {
            long long tmp = x1;
            x1 = x2;
            x2 = tmp;
          }
          if (y1 > y2) {
            long long tmp = y1;
            y1 = y2;
            y2 = tmp;
          }
          long long w = x2 - x1 + 1;
          long long h = y2 - y1 + 1;
          bool all_inside = true;

          long long area = w * h;
          if (area > largest) {
            for (long long y = y1; y < y2 && all_inside; y++) {
              for (long long x = x1; x < x2 && all_inside; x++) {
                bool odd = false;
                long long ray_source_x = x;
                long long ray_source_y = y;
                //   long long ray_target_x = x;
                long long ray_target_y = miny;
                for (size_t k = 0; k < line_count; k++) {
                  size_t kk1 = (k) % line_count;
                  size_t kk2 = (k + 1) % line_count;
                  long long lx1 = corners[kk1][0];
                  long long ly1 = corners[kk1][1];
                  long long lx2 = corners[kk2][0];
                  //    long long ly2 = corners[kk2][1];

                  bool interX = (lx1 > ray_source_x) == (lx2 < ray_source_x);
                  bool interY = (ly1 > ray_source_y) == (ly1 < ray_target_y);

                  if (interX && interY) {
                    odd = !odd;
                  }
                }
                all_inside = all_inside && odd;
              }
            }
            if (!all_inside) {
              continue;
            }

            largest = area;
          }
        }
      }
    }
  }
  return largest;
}
