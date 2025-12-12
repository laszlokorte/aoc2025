#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define TMP_FILE_NAME "tmp_problem.smt2"
#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

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

    // use z3 solver
    // create file to write the problem into
    FILE *tmp = fopen(TMP_FILE_NAME, "w");

    if (!tmp) {
      return 1;
    }
    int min = 0;
    {
      // define a variable for each button
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(tmp, "(declare-const x%lu Int)\n", bi);
      }

      fprintf(tmp, "\n");
    }
    {

      // require number of button presses to be positive
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(tmp, "(assert (>= x%lu 0))\n", bi);
      }

      fprintf(tmp, "\n");
    }

    // for each target joltage require the number
    // of relevant button presses to sum to the exact value
    for (size_t li = 0; li < 16; li++) {
      fprintf(tmp, "(assert (= (+");
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(tmp, " (* x%lu %d)", bi, toggle_mask[bi][li]);
      }

      fprintf(tmp, ") %d))", goal_light[li]);
      fprintf(tmp, "\n");
    }
    {
      // minimize the total number of button presses
      fprintf(tmp, "(minimize (+");
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(tmp, " x%lu", bi);
      }

      fprintf(tmp, "))\n");
    }
    // run the solver
    fprintf(tmp, "(check-sat)\n");
    // output the result
    fprintf(tmp, "(get-objectives)\n");
    fclose(tmp);
    // feed the problem description to z3
    FILE *fp = popen("z3 -T:0 tmp_problem.smt2", "r");
    if (!fp) {
      fprintf(stderr, "z3 could not be started");
      return 1;
    }
    char buffer[1024];
    void *read = 0;
    // read z3 output
    while ((read = fgets(buffer, sizeof(buffer), fp)) != NULL) {
      // look for lines that contain both opening and closing paren
      char *start = strchr(buffer, '(');
      char *end = strrchr(buffer, ')');
      if (start && end) {
        // result (min number of button presses)
        // is to be found after the last space
        char *last_space = strrchr(buffer, ' ');
        if (last_space && last_space++ < end) {
          // parse integer
          int accum = 0;
          while (last_space < end) {
            accum = accum * 10 + ((*last_space) - '0');
            last_space++;
          }
          min = accum;
        }
      }
    }
    pclose(fp);
    unlink(TMP_FILE_NAME);
    sum += min;
  }

  return sum;
}
