#include "day.h"
#include <stdbool.h>
#include <stdio.h>

static inline int digit(long long number, int d) {
  while (d > 0) {
    number /= 10;
    d--;
  }

  return number % 10;
}

static inline int digits(long long number) {
  int d = 0;
  while (number > 0) {
    number /= 10;
    d++;
  }
  return d;
}

static inline bool is_funny(long long number, int parts) {
  int dgits = digits(number);
  if (dgits % parts != 0) {
    return false;
  }
  int part_width = dgits / parts;
  int start = part_width;
  bool matches = true;
  for (int start = part_width; matches && start < dgits; start += part_width) {
    for (int step = 0; matches && step < part_width; step++) {
      matches = matches && (digit(number, start + step) ==
                            digit(number, start + step - part_width));
    }
  }
  return matches;
}

static inline bool is_funny_any(long long number) {
  int d = digits(number);
  for (int x = 2; x <= d; x++) {
    if (d % x != 0) {
      continue;
    }
    if (is_funny(number, x)) {
      return true;
    }
  }
  return false;
}

long long part1(size_t size, char *buffer) {
  int pos = 0;
  int r = 0;
  long long result = 0;
  while (pos <= size) {
    char c = buffer[pos];
    long long range[2];
    switch (c) {
    case '-': {
      r = 1 - r;
      pos++;
      continue;
    } break;
    case '\0':
    case ',': {
      pos++;
      if (r == 0) {
        break;
      }
      r = 1 - r;
      for (long long v = range[0]; v <= range[1]; v++) {
        if (is_funny(v, 2)) {
          result += v;
        }
      }
      range[0] = 0;
      range[1] = 0;
      continue;
    } break;
    case '\n':
    case ' ': {
      pos++;
      continue;
    } break;
    default: {
      long long accum = 0;
      while (buffer[pos] >= 48 && buffer[pos] <= 57) {
        accum = accum * 10 + (buffer[pos] - 48);
        pos++;
      }
      range[r] = accum;
    }
    }
  }

  return result;
}

long long part2(size_t size, char *buffer) {
  int pos = 0;
  int r = 0;
  long long result = 0;
  while (pos <= size) {
    char c = buffer[pos];
    long long range[2];
    switch (c) {
    case '-': {
      r = 1 - r;
      pos++;
      continue;
    } break;
    case '\0':
    case ',': {
      pos++;
      if (r == 0) {
        break;
      }
      r = 1 - r;
      for (long long v = range[0]; v <= range[1]; v++) {

        if (is_funny_any(v)) {
          result += v;
        }
      }
      range[0] = 0;
      range[1] = 0;
      continue;
    } break;
    case '\n':
    case ' ': {
      pos++;
      continue;
    } break;
    default: {
      long long accum = 0;
      while (buffer[pos] >= 48 && buffer[pos] <= 57) {
        accum = accum * 10 + (buffer[pos] - 48);
        pos++;
      }
      range[r] = accum;
    }
    }
  }

  return result;
}
