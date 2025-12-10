#include "day.h"
#include <stdbool.h>
#include <stdio.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

long long calc(size_t size, char *buffer);

long long part1(size_t size, char *buffer) { return calc(size, buffer); }
long long part2(size_t size, char *buffer) { return calc(size, buffer); }

long long calc(size_t size, char *buffer) {
  long long sum = 0;
  size_t line_count = 1;
  {
    // count total lines
    size_t pos = 0;
    while (buffer[pos] != '\0' && pos <= size) {
      if (buffer[pos] == '\n') {
        line_count++;
      }
      pos++;
    }
  }

  size_t p = 0;
  for (size_t l = 0; l < line_count; l++) {
    // bitset of turnd on lights
    unsigned int goal_light = 0;
    {
      // parse string between [ and ] into bitset
      if (buffer[p] == '[') {
        p++;
      }
      size_t c = 0;
      while (buffer[p] != ']' && p <= size) {
        goal_light = goal_light | ((buffer[p] == '#') << c);
        p++;
        c++;
      }
      p++;
      p++;
    }
    size_t buttons = 0;
    size_t button_start = p;
    { // count number of buttons (the number of parenthesis)
      while (buffer[p] == '(' && p <= size) {
        buttons++;
        while (buffer[p] != ')' && p <= size) {
          p++;
        }
        p++;
        p++;
      }
      while (buffer[p] != '\n' && p <= size) {
        p++;
      }
      p++;
    }
    // each button being a binary trigger can be pressed at most once
    // pressing twice is the same as pressing not at all
    // So we need to try at most pow(2, buttons) combinations
    size_t combinations = 2 << buttons;
    // init minium to a large number
    long min = 2 * combinations;

    // Try all pow(2, buttons) combinations
    for (unsigned int comb = 0; comb < combinations; comb++) {
      unsigned int current_light_state = 0; // the currenty state of the light
      unsigned int toggle_count = 0;        // n
      size_t pp = button_start; // reset cursor the position where the first "("
                                // in the line is
      unsigned int button_number = 0;
      // parse buttons
      while (buffer[pp] == '(' && pp <= size) {
        unsigned int toggle_mask = 0;
        pp++;
        // parse each element of the button
        while (buffer[pp] != ' ' && pp <= size) {
          size_t accum = 0;
          while (buffer[pp] != ')' && buffer[pp] != ',' && pp <= size) {
            accum = accum * 10 + (buffer[pp] - '0');
            pp++;
          }
          // turn the button labels into a bitset
          toggle_mask = toggle_mask | (1 << accum);

          pp++;
        }
        // check if we should try to press the button in the
        // current iteration
        if (comb & (1 << button_number)) {
          toggle_count++;
          current_light_state = current_light_state ^ toggle_mask;
        }
        button_number++;

        pp++;
      }
      // check if the light state matches our goal
      // and if the number of needed button presses was lower than before
      if (current_light_state == goal_light && toggle_count < min) {
        min = toggle_count;
      }
    }
    sum += min;
  }

  return sum;
}
