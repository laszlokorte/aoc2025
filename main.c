#include <stdio.h>
#include <stdlib.h>

#include "day.h"

#ifdef DAY
#include XSTR(DAY.c)
#else
#endif

int main(int argc, char **args) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [input_file]\n", args[0]);
  }
  for (int i = 1; i < argc; i++) {
    char *filename = args[i];
    FILE *f = fopen(filename, "r");
    if (!f) {
      fprintf(stderr, "file not found: %s\n", filename);
      continue;
    }
    if (fseek(f, 0L, SEEK_END) != 0) {
      fprintf(stderr, "fseek failed");
      fclose(f);
      continue;
    }
    long size = ftell(f);
    if (size < 0) {
      fprintf(stderr, "ftell failed");
      fclose(f);
      continue;
    }
    rewind(f);
    char *content = (char *)malloc(size + 1);
    if (!content) {
      fprintf(stderr, "Could not allocate memory to read file");
      fclose(f);
      continue;
    }
    long r = fread(content, 1, size, f);
    if (r != size) {
      fprintf(stderr, "fread failed for %s\n", filename);
      free(content);
      fclose(f);
      continue;
    }
    content[size] = 0;
    fclose(f);

    long long result1 = part1(size, content);
    long long result2 = part2(size, content);

    if (result1 || result2) {
    }
#ifndef NOOUTPUT
    printf("result part1 [%s]: %llu\n", filename, result1);
    printf("result part2 [%s]: %llu\n", filename, result2);
#endif
    free(content);
  }
  return 0;
}
