#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
struct search_state {
  int presses;
  int missing_total;
  unsigned int light_state[16];
};
long dfs(unsigned int toggle_mask[32][16], size_t current_button,
         size_t button_count, struct search_state state, long *cache);
long long part1(size_t size, char *buffer) {
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
      unsigned int button_count = 0;
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
        if (comb & (1 << button_count)) {
          toggle_count++;
          current_light_state = current_light_state ^ toggle_mask;
        }
        button_count++;

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
#define CACHE_SIZE 1234556712
long long part2(size_t size, char *buffer) {
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
  long *cache = (long *)malloc(sizeof(long) * CACHE_SIZE);
  memset(cache, 0, sizeof(long) * CACHE_SIZE);
  size_t p = 0;
  for (size_t l = 0; l < line_count; l++) {
    // printf("xxx\n");
    // bitset of turnd on lights
    unsigned int goal_light[16] = {};
    {
      size_t line_start = p;
      // parse string between [ and ] into bitset
      while (buffer[p] != '{') {
        p++;
      }
      p++;
      size_t gg = 0;
      while (buffer[p] != '\n' && p < size) {
        size_t accum = 0;
        while (buffer[p] != '}' && buffer[p] != ',' && p <= size) {
          accum = accum * 10 + (buffer[p] - '0');
          p++;
        }
        // turn the button labels into a bitset
        goal_light[gg++] = accum;
        p++;
      }
      p = line_start;
      while (buffer[p] != ']') {
        p++;
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
    // unsigned int current_light_state[16] =
    //    {};                   // the currenty state of the light
    size_t pp = button_start; // reset cursor the position where the first "("
                              // in the line is
    unsigned int button_count = 0;
    // parse buttons
    unsigned int toggle_mask[32][16] = {};
    while (buffer[pp] == '(' && pp <= size) {
      pp++;
      // parse each element of the button
      while (buffer[pp] != ' ' && pp <= size) {
        size_t accum = 0;
        while (buffer[pp] != ')' && buffer[pp] != ',' && pp <= size) {
          accum = accum * 10 + (buffer[pp] - '0');
          pp++;
        }
        // turn the button labels into a bitset
        toggle_mask[button_count][accum] = 1;

        pp++;
      }
      button_count++;

      pp++;
    }

    //   printf("buttons: \n");
    for (size_t bi = 0; bi < button_count; bi++) {
      for (size_t li = 0; li < 16; li++) {
        //   printf("%d,", toggle_mask[bi][li]);
      }
      //    printf("\n");
    }
    // printf("goal: \n");
    for (size_t li = 0; li < 16; li++) {
      //     printf("%d,", goal_light[li]);
    }
    //    printf("\n");

    struct search_state state = {};
    state.presses = 0;
    for (size_t g = 0; g < 16; g++) {
      state.light_state[g] = goal_light[g];
      state.missing_total += goal_light[g];
    }
    printf("start dfs\n");

    // printf("initial missing total: %d, %d\n", state.missing_total,
    //       button_count);
    //
    memset(cache, 0, sizeof(long) * CACHE_SIZE);
    sum += dfs(toggle_mask, 0, button_count, state, cache);
  }

  return sum;
}
long dfs(unsigned int toggle_mask[32][16], size_t current_button,
         size_t button_count, struct search_state state, long *cache) {

  // printf("%lu/%lu, %d\n", current_button, button_count,
  // state.missing_total);
  if (state.missing_total < 0) {
    return 99999;
  } else if (state.missing_total == 0) {
    return state.presses;
  } else {
    if (current_button >= button_count) {
      return 99999;
    }
    struct search_state new_state = {};
    new_state.missing_total = state.missing_total;
    new_state.presses = state.presses + 1;

    for (size_t g = 0; g < 16; g++) {
      if (toggle_mask[current_button][g] > state.light_state[g]) {
        new_state.missing_total = -1;
        break;
      }
      new_state.light_state[g] =
          state.light_state[g] - toggle_mask[current_button][g];
      new_state.missing_total -= toggle_mask[current_button][g];
    }
    if (new_state.missing_total == 0) {
      bool valid = true;
      for (size_t g = 0; g < 16 && valid; g++) {
        if (new_state.light_state[g] != 0) {
          valid = false;
        }
      }
      if (valid) {
        return new_state.presses;
      } else {
        long min =
            dfs(toggle_mask, current_button + 1, button_count, state, cache);
        return min;
      }
    } else if (new_state.missing_total >= 0) {
      // printf("xxx %d\n", new_state.missing_total);

      if (current_button <= button_count) {
        long only_next_button =
            dfs(toggle_mask, current_button + 1, button_count, state, cache);
        long same_button =
            dfs(toggle_mask, current_button, button_count, new_state, cache);
        long next_button = dfs(toggle_mask, current_button + 1, button_count,
                               new_state, cache);
        long min = MIN(only_next_button, MIN(same_button, next_button));

        return min;
      } else {
        long min =
            dfs(toggle_mask, current_button, button_count, new_state, cache);

        return min;
      }
    } else {
      long min =
          dfs(toggle_mask, current_button + 1, button_count, state, cache);

      return min;
    }
  }
}
