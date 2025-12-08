#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    fseek(f, 0L, SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    char *content = (char *)malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = 0;
    fclose(f);

    printf("result part1 [%s]: %llu\n", filename, part1(size, content));
    printf("result part2 [%s]: %llu\n", filename, part2(size, content));
    free(content);
  }
  return 0;
}
