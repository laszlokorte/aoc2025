#include "day.h"
#include <stdbool.h>
#include <stdio.h>

#define DEBUG(a, b) {};
//#define DEBUG(a, b) printf(a, b);

long long calc(size_t size, char *buffer, bool remove);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }

long long calc(size_t size, char *buffer, bool remove) {
  long long reachable = 0;
  int line_length = 0;
  while (buffer[line_length] != '\n' && buffer[line_length] != '\0' &&
         line_length <= (int)size) {
    line_length++;
  }
  int line_count = size / line_length;

  while (1) {
    int removed = 0;
    for (int y = 0; y < line_count; y++) {
      for (int x = 0; x < line_length; x++) {
        int i = x + line_length * y + y;
        int neighbour_rolls = 0;
        if (buffer[i] == '@') {
          for (int yy = -1; yy <= 1; yy++) {
            for (int xx = -1; xx <= 1; xx++) {
              int xxx = x + xx;
              int yyy = y + yy;
              int pos = xxx + line_length * yyy + yyy;
              if (xx == 0 && yy == 0) {
                continue;
              }
              if (xxx < 0 || xxx >= line_length) {
                continue;
              }
              if (yyy < 0 || yyy >= line_count) {
                continue;
              }
              if (buffer[pos] != '@') {
                continue;
              }
              neighbour_rolls++;
            }
          }
          if (neighbour_rolls < 4) {
            reachable++;
            if (remove) {
              buffer[i] = 'x';
              removed++;
            }
          }
        }
        DEBUG("%c", buffer[i]);
      }
      DEBUG("%c", '\n');
    }
    if (removed == 0) {
      break;
    }
    DEBUG("%c", '\n');
  }

  return reachable;
}
