#include "day.h"
#include <stdbool.h>
#include <stdio.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

long long calc(size_t size, char *buffer, bool count_paths);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }

long long calc(size_t size, char *buffer, bool count_paths) {
  long long split_count = 0;
  int width = 0;
  int pos = 0;
  while (buffer[pos] != '\n' && pos <= (int)size) {
    width++;
    pos++;
  }
  int stride = width + 1;
  int height = (size + 1) / stride;

  // scratchpad for storing
  // the total path counts for the previous and
  // current row
  long long possibilities[2][width];

  for (int c = 0; c < width; c++) {
    possibilities[0][c] = 0;
    possibilities[1][c] = 0;
  }

  // walk top to bottom
  for (int y = 1; y < height; y++) {
    int this_row = y % 2;
    int prev_row = (y + 1) % 2;

    // reset possibility counter
    for (int xx = 0; xx < width; xx++) {
      possibilities[this_row][xx] = 0;
    }

    // walk left to right
    for (int x = 0; x < width; x++) {
      size_t here = x + y * stride;
      size_t above = x + (y - 1) * stride;
      size_t left = (x - 1) + y * stride;
      size_t right = (x + 1) + y * stride;

      // check tile above
      switch (buffer[above]) {
      case 'S':
        // initialize path count for cell below S
        possibilities[prev_row][x] = 1;
      case '|': {
        // ray coming from above
        switch (buffer[here]) {
        case '|':
        case '.': {
          // set current cell to ray
          buffer[here] = '|';
          // and increase path count by aboves path count
          possibilities[(y % 2)][x] += possibilities[prev_row][x];
        } break;
        case '^': {
          // propagate ray to neighors
          buffer[left] = '|';
          buffer[right] = '|';
          // increases split count
          split_count++;
          // increase path count
          possibilities[this_row][x - 1] += possibilities[prev_row][x];
          possibilities[this_row][x + 1] += possibilities[prev_row][x];
        } break;
        }

      } break;
      }
    }
  }
  if (count_paths) {
    long long path_count = 0;
    for (int xx = 0; xx < width; xx++) {
      path_count += possibilities[1][xx];
    }
    return path_count;
  } else {
    return split_count;
  }
}
