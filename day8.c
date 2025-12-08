#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

long long calc(size_t size, char *buffer, bool to_the_end);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }
int compare_edge_length(void *l, const void *a, const void *b);
size_t uf_find(size_t *parent, size_t x);
bool uf_union(size_t *parent, size_t *size, size_t x, size_t y);
union box {
  struct xyz {
    int x;
    int y;
    int z;
  } xyz;
  int coords[3];
};

long long calc(size_t size, char *buffer, bool to_the_end) {
  size_t line_count = 1;
  int pos = 0;
  while (buffer[pos] != '\0' && pos <= (int)size) {
    if (buffer[pos] == '\n') {
      line_count++;
    }
    pos++;
  }

  union box boxes[line_count];
  int c = 0;
  for (size_t l = 0; l < line_count; l++) {
    union box b = {};
    for (size_t d = 0; d < 3; d++) {
      int accum = 0;
      while (buffer[c] != '\0' && c <= (int)size && buffer[c] != '\n' &&
             buffer[c] != ',') {
        accum = accum * 10 + (buffer[c] - '0');
        c++;
      }
      b.coords[d] = accum;
      c++;
    }

    boxes[l] = b;
  }
  size_t node_count = line_count;
  size_t edge_count = node_count * node_count;

  long long *edge_lengths = malloc(edge_count * sizeof(*edge_lengths));
  size_t *edge_ids = malloc(edge_count * sizeof(*edge_ids));
  for (size_t a = 0; a < node_count; a++) {
    for (size_t b = 0; b < node_count; b++) {
      long long length = 0;
      for (int d = 0; d < 3; d++) {
        long long sub = boxes[b].coords[d] - boxes[a].coords[d];
        length += sub * sub;
      }

      size_t edge_index = b * line_count + a;

      edge_lengths[edge_index] = length;
      DEBUG("%lu - %lu: %lld (%d,%d,%d);(%d,%d,%d)\n", a, b, length,
            boxes[a].coords[0], boxes[a].coords[1], boxes[a].coords[2],
            boxes[b].coords[0], boxes[b].coords[1], boxes[b].coords[2]);
      edge_ids[edge_index] = edge_index;
    }
  }

  qsort_r(edge_ids, edge_count, sizeof(size_t), edge_lengths,
          &compare_edge_length);

  size_t *union_find_parent = malloc(node_count * sizeof(size_t));
  size_t *union_find_size = malloc(node_count * sizeof(size_t));

  for (size_t u = 0; u < node_count; u++) {
    union_find_parent[u] = u;
    union_find_size[u] = 1;
  }
  int remaining_trys = node_count < 50 ? 10 : 1000;
  for (size_t e = 0; e < edge_count; e++) {
    if (!to_the_end && remaining_trys <= 0) {
      break;
    }
    size_t edge_id = edge_ids[e];
    size_t n1 = edge_id % node_count;
    size_t n2 = edge_id / node_count;
    // Skip reflexive Edges
    if (n1 >= n2) {
      continue;
    }

    DEBUG("%d, try connect %lu to %lu\n", remaining_trys, n1, n2);
    remaining_trys--;
    if (!uf_union(union_find_parent, union_find_size, n1, n2)) {
      DEBUG("already connected\n");
    }
    if (to_the_end && union_find_size[uf_find(union_find_parent, n1)] == node_count) {
      return boxes[n1].xyz.x * boxes[n2].xyz.x;
    }
  }

  size_t product = 1;
  size_t max[3] = {1, 1, 1};

  for (size_t m = 0; m < 3; m++) {
    for (size_t u = 0; u < node_count; u++) {
      if (u != uf_find(union_find_parent, u)) {
        continue;
      }

      if (max[m] < union_find_size[u] &&
          (m == 0 || union_find_size[u] < max[m - 1])) {
        max[m] = union_find_size[u];
      }
    }
    product *= max[m];
  }

  return product;
}
int compare_edge_length(void *l, const void *a, const void *b) {
  long long *edge_lengths = (long long *)l;
  size_t e1 = *(size_t *)a;
  size_t e2 = *(size_t *)b;
  long long l1 = edge_lengths[e1];
  long long l2 = edge_lengths[e2];

  if (l1 > l2) {
    return 1;
  } else if (l1 < l2) {
    return -1;
  } else {
    return 0;
  }
}

size_t uf_find(size_t *parent, size_t x) {
  if (parent[x] != x)
    parent[x] = uf_find(parent, parent[x]);
  return parent[x];
}
bool uf_union(size_t *parent, size_t *size, size_t x, size_t y) {
  x = uf_find(parent, x);
  y = uf_find(parent, y);
  if (x == y)
    return false; // Zyklus / schon verbunden

  // Union by Size
  if (size[x] < size[y]) {
    size_t tmp = x;
    x = y;
    y = tmp;
  }

  parent[y] = x;
  size[x] += size[y];
  return true;
}
