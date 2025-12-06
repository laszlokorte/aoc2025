#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);
long long calc(size_t size, char *buffer);

long long part1(size_t size, char *buffer) {
  long long total_sum = 0;

  int line_length = 0;
  int line_start = 0;
  int line_count = 0;
  int pos = 0;
  while (buffer[pos] != '\0' && pos <= (int)size) {
    if (buffer[pos] == '\n') {
      line_length = 1;
      line_count++;
      line_start = pos + 1;
    }
    line_length++;
    pos++;
  }

  int p = line_start;
  while (buffer[p] != '\0' && p <= (int)size) {
    char op = buffer[p];
    int c = p - line_start;
    int neutral_element = -1;
    switch (op) {
    case '+': {
      neutral_element = 0;
    } break;
    case '*': {
      neutral_element = 1;
    } break;
    }
    if (op == '*' || op == '+') {
      long long column_result = neutral_element;
      DEBUG("%c\n", op);
      for (int l = 0; l < line_count; l++) {
        int digit_pos = 0;
        long long number_accum = 0;
        while (buffer[c + line_length * l + digit_pos] == ' ') {
          digit_pos++;
        }
        while (buffer[c + line_length * l + digit_pos] != ' ' &&
               buffer[c + line_length * l + digit_pos] != '\n') {
          int digit = buffer[c + line_length * l + digit_pos] - '0';
          number_accum = number_accum * 10 + digit;
          digit_pos++;
        }
        DEBUG("%llu\n", number_accum);
        switch (op) {
        case '+': {
          column_result += number_accum;
        } break;
        case '*': {
          column_result *= number_accum;
        } break;
        }
      }
      DEBUG("-> %llu\n", column_result);
      total_sum += column_result;
    }
    p++;
  }

  return total_sum;
}

long long part2(size_t size, char *buffer) {
  unsigned long long total_sum = 0;

  int line_length = 0;
  int line_start = 0;
  int line_count = 0;
  int pos = 0;
  while (buffer[pos] != '\0' && pos <= (int)size) {
    if (buffer[pos] == '\n') {
      line_length = 1;
      line_count++;
      line_start = pos + 1;
    }
    line_length++;
    pos++;
  }

  int p = line_start;
  while (buffer[p] != '\0' && p <= (int)size) {
    char op = buffer[p];
    int c = p - line_start;
    int neutral_element = -1;
    switch (op) {
    case '+': {
      neutral_element = 0;
    } break;
    case '*': {
      neutral_element = 1;
    } break;
    }
    if (op == '*' || op == '+') {
      long long column_result = neutral_element;
      DEBUG("%c\n", op);
      int digit_pos = 0;
      while (true) {
        long long number_accum = 0;
        for (int l = 0; l < line_count; l++) {
          int digit = buffer[c + line_length * l + digit_pos] - '0';
          if (digit < 0 || digit > 10) {
            if (number_accum == 0) {
              continue;
            } else {
              break;
            }
          }
          number_accum = number_accum * 10 + digit;
        }

        DEBUG(" %llu\n", number_accum);
        if (!number_accum) {
          break;
        }
        DEBUG("-> %llu\n", number_accum);
        switch (op) {
        case '+': {
          column_result += number_accum;
        } break;
        case '*': {
          column_result *= number_accum;
        } break;
        }
        digit_pos++;
      }
      total_sum += column_result;
    }
    p++;
  }

  //  printf("total: %llu\n", total_sum);
  return total_sum;
}
