#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define CACHE_SIZE 100003
#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

typedef struct cache_entry {
  unsigned long key;
  int value; // 0 = not solvable, 1 = solvable
  struct cache_entry *next;
} cache_entry;

int dfs(unsigned short toggle_mask[32][16], unsigned int remain[16], size_t idx,
        size_t button_count, cache_entry **cache);
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
  cache_entry **cache = (cache_entry **)malloc(sizeof(size_t) * CACHE_SIZE);
  size_t p = 0;
  for (size_t l = 0; l < line_count; l++) {
    // DEBUG("xxx\n");
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
    unsigned short toggle_mask[32][16] = {};
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

    DEBUG("buttons: \n");
    for (size_t bi = 0; bi < button_count; bi++) {
      for (size_t li = 0; li < 16; li++) {
        DEBUG("%d,", toggle_mask[bi][li]);
      }
      DEBUG("\n");
    }
    DEBUG("goal: \n");
    for (size_t li = 0; li < 16; li++) {
      DEBUG("%d,", goal_light[li]);
    }
    DEBUG("\n");

    DEBUG("start search\n");

    // DEBUG("initial missing total: %d, %d\n", state.missing_total,
    //       button_count);
    //
    memset(cache, 0, sizeof(size_t) * CACHE_SIZE);
    int min = dfs(toggle_mask, goal_light, 0, button_count, cache);
    DEBUG("%d, %d\n", min, button_count);

    sum += min;
  }

  return sum;
}

unsigned long hash_state(unsigned int remain[16]) {
  unsigned long h = 0;
  for (int i = 0; i < 16; i++) {
    h = h * 31 + remain[i];
  }
  return h;
}

// DFS returning minimal number of vectors to reach 'remain'
int dfs(unsigned short toggle_mask[32][16], unsigned int remain[16], size_t idx,
        size_t button_count, cache_entry **cache) {
  int done = 1;
  for (int i = 0; i < 16; i++) {
    if (remain[i] != 0) {
      done = 0;
      break;
    }
  }
  if (done) {
    return 0; // 0 vectors needed for empty target
  }

  if (idx >= button_count)
    return -1; // no solution

  // cache lookup
  unsigned long h = hash_state(remain) ^ idx;
  unsigned long slot = h % CACHE_SIZE;
  for (cache_entry *e = cache[slot]; e; e = e->next) {
    if (e->key == h)
      return e->value;
  }
  int min_count = -1;

  // compute max times we can use this vector
  int max_take = -1;
  for (int i = 0; i < 16; i++) {
    if (toggle_mask[idx][i] == 0)
      continue; // ignore 0 components
    int t = remain[i] / toggle_mask[idx][i];
    if (max_take == -1 || t < max_take)
      max_take = t;
  }

  // if max_take == -1, the vector is all zeros, only 0 copies can be used
  if (max_take == -1) {
    max_take = 0;
  }

  for (int take = 0; take <= max_take; take++) {
    // subtract take copies of the vector
    for (int i = 0; i < 16; i++) {
      remain[i] -= take * toggle_mask[idx][i];
    }

    int res = dfs(toggle_mask, remain, idx + 1, button_count, cache);
    if (res >= 0) {
      int total = res + take;
      if (min_count == -1 || total < min_count) {
        min_count = total;
      }
    }

    // backtrack
    for (int i = 0; i < 16; i++) {
      remain[i] += take * toggle_mask[idx][i];
    }
  }

  // store in cache
  cache_entry *e = malloc(sizeof(cache_entry));
  e->key = h;
  e->value = min_count;
  e->next = cache[slot];
  cache[slot] = e;

  return min_count;
}
