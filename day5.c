#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

long long calc(size_t size, char *buffer, bool all_in_ranges);
int compare_range(const void *a, const void *b);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }

struct range {
  long long start;
  long long end;
};

long long calc(size_t size, char *buffer, bool all_in_ranges) {
  long long fresh = 0;

  // Count total number of ranges
  int num_ranges = 1;
  int pos = 0;
  while (buffer[pos] != '\0' && pos <= (int)size &&
         (buffer[pos] != '\n' || buffer[pos + 1] != '\n')) {
    if (buffer[pos] == '\n') {
      num_ranges++;
    }
    pos++;
  }

  // collect ranges into custom structure
  struct range ranges[num_ranges];
  int i = 0;
  for (int r = 0; r < num_ranges; r++) {
    struct range new_range = {0};
    // parse range start
    while (buffer[i] != '\0' && pos <= (int)size && (buffer[i] != '-')) {
      int d = buffer[i] - '0';
      new_range.start = new_range.start * 10 + d;
      i++;
    }
    i++;
    // parse range end
    while (buffer[i] != '\0' && pos <= (int)size && (buffer[i] != '\n')) {
      int d = buffer[i] - '0';
      new_range.end = new_range.end * 10 + d;
      i++;
    }
    i++;
    ranges[r] = new_range;
  }

  if (!all_in_ranges) {
    // for part one, parse the ingredients and match each against list of ranges
    for (int r = 0; r < num_ranges; r++) {
      DEBUG("%llu-%llu\n", ranges[r].start, ranges[r].end);
    }
    while (buffer[i] != '\0' && pos <= (int)size) {
      long long acc = 0;
      // parse decimal number
      while (buffer[i] != '\0' && i <= (int)size && buffer[i] != '\n') {
        int d = buffer[i] - '0';
        acc = acc * 10 + d;
        i++;
      }

      i++;
      DEBUG("%llu\n", acc);
      // linear search in list of ranges
      for (int r = 0; r < num_ranges; r++) {
        struct range range = ranges[r];
        if (range.start <= acc && acc <= range.end) {
          DEBUG("fresh: %llu\n", acc);
          fresh++;
          break;
        }
      }
    }
  } else {
    // sort ranges lexicographically by start and end
    qsort(ranges, num_ranges, sizeof(struct range), compare_range);
    // scanline walk through the sorted ranges
    long long cursor = 0;
    for (int r = 0; r < num_ranges; r++) {
      struct range rn = ranges[r];
      if (cursor < rn.start) { // next range starts after our cursor, no overlap
        DEBUG("overlap: cursor: %lli,  end: %lli, grow: %lli\n", cursor, rn.end,
              rn.end - rn.start + 1);
        fresh += rn.end - rn.start + 1;
        cursor = rn.end;

      } else if (cursor >= rn.start &&
                 cursor < rn.end) { // cursor is already ahead of range start
                                    // but before range end: overlap
        DEBUG("overlap: cursor: %lli,  end: %lli, grow: %lli\n", cursor, rn.end,
              rn.end - cursor);
        fresh += rn.end - cursor;
        cursor = rn.end;
      }
    }
  }

  return fresh;
}

// helper to sort ranges lexicographically
int compare_range(const void *a, const void *b) {
  struct range r1 = *(struct range *)a;
  struct range r2 = *(struct range *)b;
  if (r1.start < r2.start)
    return -1;
  if (r1.start == r2.start && r1.end < r2.end)
    return -1;
  if (r1.start == r2.start && r1.end == r2.end)
    return 0;
  return 1;
}
