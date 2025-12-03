#include "day.h"
#include <stdbool.h>
#include <stdio.h>

long long calc(size_t size, char *buffer, int max_digits);

long long part1(size_t size, char *buffer) { return calc(size, buffer, 2); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, 12); }

long long calc(size_t size, char *buffer, int max_digits) {
  long long sum = 0;
  size_t line_start = 0;
  while (line_start <= size) {
    size_t line_length = 0;
    while (buffer[line_length] != '\n' && buffer[line_length] != '\0') {
      line_length++;
    }
    line_length++;

    int found_digits = 0;
    long long voltage = 0;
    size_t start_pos = 0;
    while (found_digits < max_digits) {
      int best_digit = 0;
      size_t last_valid_pos = line_length - max_digits + found_digits;
      for (size_t pos = start_pos; pos < last_valid_pos; pos++) {
        int d = buffer[line_start + pos] - 48;
        if (d > best_digit) {
          best_digit = d;
          start_pos = pos + 1;
        }
      }
      voltage = voltage * 10 + best_digit;
      found_digits++;
    }
    sum += voltage;
    line_start += line_length;
  }

  return sum;
}
