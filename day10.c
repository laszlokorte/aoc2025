#include "day.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_BUTTONS 32
#define MAX_DIMS 16

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
    unsigned int goal_light[MAX_DIMS] = {};
    {
      size_t line_start = p;
      // parse string between { and } into bitset
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
    size_t button_start = p;
    {
      // count number of buttons (the number of parenthesis)
      while (buffer[p] == '(' && p <= size) {
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

    unsigned short toggle_mask[MAX_BUTTONS][MAX_DIMS] = {};
    unsigned int button_count = 0;
    {
      // reset cursor the position where the first "("
      // in the line is
      size_t pp = button_start;
      // parse buttons
      while (buffer[pp] == '(' && pp <= size) {
        pp++;
        // parse each element of the button
        while (buffer[pp] != ' ' && pp <= size) {
          size_t accum = 0;
          while (buffer[pp] != ')' && buffer[pp] != ',' && pp <= size) {
            accum = accum * 10 + (buffer[pp] - '0');
            pp++;
          }
          // turn the button labels vector
          toggle_mask[button_count][accum] = 1;

          pp++;
        }
        button_count++;

        pp++;
      }
    }

    // use external z3 solver via subprocess
    //
    // setup pipes to write/read to z3 process
    // parent writes -> child reads
    int in_pipe[2];
    // child writes -> parent reads
    int out_pipe[2];
    // in_pipe[0] is for reading data thats writtes to in_pipe[1]
    pipe(in_pipe);
    // in_pipe[0] is for reading data thats writtes to in_pipe[1]
    pipe(out_pipe);

    // fork to launch z3
    pid_t child_pid = fork();
    if (child_pid == 0) {
      // child
      dup2(in_pipe[0], STDIN_FILENO);   // redirect in_pipe to stdin of z3
      dup2(out_pipe[1], STDOUT_FILENO); // redirect stdout of z3 to out_pipe
      close(out_pipe[0]); // we do not want to read from the out_pipe
      close(in_pipe[1]);  // we do not want to write to the in_pipe
      // start z3
      execlp("z3", "z3", "-in", "-smt2", NULL);
      // exit if done
      _exit(1);
    }

    // we do not want to read from in_pipe, thats for z3 to read from
    close(in_pipe[0]);
    // we do not want to write into out_pipe, thats for z3 to write into
    close(out_pipe[1]);

    // open in_pipe for writing input into z3
    FILE *to_z3 = fdopen(in_pipe[1], "w");
    // open out_pipe for reading from z3 output
    FILE *from_z3 = fdopen(out_pipe[0], "r");

    if (!to_z3 || !from_z3) {
      fprintf(stderr, "Error fdopen\n");
      return 1;
    }
    {
      // define a variable for each button
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(to_z3, "(declare-const x%lu Int)\n", bi);
      }

      fprintf(to_z3, "\n");
    }
    {

      // require number of button presses to be positive
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(to_z3, "(assert (>= x%lu 0))\n", bi);
      }

      fprintf(to_z3, "\n");
    }

    // for each target joltage require the number
    // of relevant button presses to sum to the exact value
    for (size_t li = 0; li < MAX_DIMS; li++) {
      fprintf(to_z3, "(assert (= (+");
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(to_z3, " (* x%lu %d)", bi, toggle_mask[bi][li]);
      }

      fprintf(to_z3, ") %d))", goal_light[li]);
      fprintf(to_z3, "\n");
    }

    // define the goal
    {
      // minimize the total number of button presses
      fprintf(to_z3, "(minimize (+");
      for (size_t bi = 0; bi < button_count; bi++) {
        fprintf(to_z3, " x%lu", bi);
      }

      fprintf(to_z3, "))\n");
    }
    // run the solver
    fprintf(to_z3, "(check-sat)\n");
    // output the result
    fprintf(to_z3, "(get-objectives)\n");
    // make z3 exit when done
    fprintf(to_z3, "(exit)\n");
    fflush(to_z3);

    char buffer[1024];
    void *read = 0;
    // read z3 output
    //
    int min = 0;
    while ((read = fgets(buffer, sizeof(buffer), from_z3)) != NULL) {
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
          // we already have the result, force kill fork if not gone yet.
          kill(child_pid, SIGKILL);
          waitpid(child_pid, NULL, 0);
          break;
        }
      }
    }

    // close file descriptors
    fclose(from_z3);
    fclose(to_z3);
    sum += min;
  }

  return sum;
}
