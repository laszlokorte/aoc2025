#include "day.h"
#include <stdbool.h>
#include <stdio.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

long long calc(size_t size, char *buffer, bool count_paths);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }

long long calc(size_t size, char *buffer, bool count_paths) {
  long long total_sum = 0;
  int width = 0;
  int pos = 0;
  while (buffer[pos] != '\n' && pos <= (int)size) {
    width++;
    pos++;
  }

  long long possibilities[2][width];

  for (int xx = 0; xx < width; xx++) {
    possibilities[0][xx] = 0;
    possibilities[1][xx] = 0;
  }

  int height = (size + 1) / (width + 1);
  for (int y = 1; y < height; y++) {
      // reset possibility counter
    for (int xx = 0; xx < width; xx++) {
      possibilities[(y) % 2][xx] = 0;
    }
    for (int x = 0; x < width; x++) {
      size_t here = x + y * (width + 1);
      size_t above = x + (y - 1) * (width + 1);
      size_t left = (x - 1) + (y) * (width + 1);
      size_t right = (x + 1) + (y) * (width + 1);
      switch (buffer[above]) {
      case 'S':
        possibilities[((y + 1) % 2)][x] = 1;
      case '|': {
        switch (buffer[here]) {
        case '|':
        case '.': {
          buffer[here] = '|';
          possibilities[(y % 2)][x] += possibilities[((y + 1) % 2)][x];
        } break;
        case '^': {
          total_sum++;
          buffer[left] = '|';
          buffer[right] = '|';
          possibilities[((y) % 2)][x - 1] += possibilities[((y + 1) % 2)][x];
          possibilities[((y) % 2)][x + 1] += possibilities[((y + 1) % 2)][x];
        } break;
        }

      } break;
      case '.': {

      } break;
      }
    }
  }
  if (count_paths) {
    long long possibility_sum = 0;
    for (int xx = 0; xx < width; xx++) {
      possibility_sum += possibilities[1][xx];
    }
    return possibility_sum;
  } else {
    return total_sum;
  }
}
