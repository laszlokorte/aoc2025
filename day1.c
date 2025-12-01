#include "day.h"
#include <stdio.h>

int part1(size_t size, char *buffer) {
  int zero = 0;
  int pos = 0;
  int sign = 0;
  int value = 50;
  while (pos < size) {

    char c = buffer[pos];
    switch (c) {
    case 'R': {
      sign = +1;
      pos++;
      continue;
    } break;
    case 'L': {
      sign = -1;
      pos++;
      continue;
    } break;
    case '\n': {
      pos++;
      continue;
    } break;
    case '\0': {
      pos++;
      continue;
    } break;
    case ' ': {
      pos++;
      continue;
    } break;
    default: {
      int accum = 0;
      while (buffer[pos] >= 48 && buffer[pos] <= 57) {
        accum = accum * 10 + (buffer[pos] - 48);
        pos++;
      }
      value = ((value + sign * accum) % 100 + 100) % 100;
      if (value == 0) {
        zero++;
      }
    }
    }
  }
  return zero;
}

int part2(size_t size, char *buffer) {
  int zero = 0;
  int pos = 0;
  int sign = 0;
  int value = 50;
  while (pos < size) {
    char c = buffer[pos];
    switch (c) {
    case 'R': {
      sign = +1;
      pos++;
      continue;
    } break;
    case 'L': {
      sign = -1;
      pos++;
      continue;
    } break;
    case '\n': {
      pos++;
      continue;
    } break;
    case '\0': {
      pos++;
      continue;
    } break;
    case ' ': {
      pos++;
      continue;
    } break;
    default: {
      int accum = 0;
      while (buffer[pos] >= 48 && buffer[pos] <= 57) {
        accum = accum * 10 + (buffer[pos] - 48);
        pos++;
      }
      int turns = accum;
      while (turns > 0) {
        turns--;
        value += sign;
        value = (value % 100 + 100) % 100;
        if (value == 0) {
          zero++;
        }
      }
    }
    }
  }
  return zero;
}
