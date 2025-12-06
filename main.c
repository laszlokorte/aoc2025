#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "day.h"

#ifdef DAY
#define DAY_DIR XSTR(DAY)
#include XSTR(DAY.c)
#else
#define DAY_DIR "."
#endif

int main(int argc, char **args) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [example|prod]\n", args[0]);
  }
  char *day = DAY_DIR;
  size_t day_length = strlen(day);
  for (int i = 1; i < argc; i++) {
    char *filename = args[i];
    size_t filename_length = strlen(filename);
    char with_extension[day_length + 1 + filename_length + 5];
    sprintf(with_extension, "%s/%s.txt", day, filename);

    printf("%s\n", with_extension);
    FILE *f = fopen(with_extension, "r");
    if (!f) {
      fprintf(stderr, "file not found: %s\n", with_extension);
      continue;
    }
    fseek(f, 0L, SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    char *content = (char *)malloc(size + 1);
    fread(content, 1, size, f);
    content[size - 1] = 0;
    fclose(f);

    printf("result part1 [%s]: %llu\n", filename, part1(size, content));
    printf("result part2 [%s]: %llu\n", filename, part2(size, content));
    free(content);
  }
  return 0;
}
