#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

long long calc(size_t size, char *buffer, bool to_the_end);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }

// helper function: compare edge ids by their edges length
int compare_edge_length(void *l, const void *a, const void *b);

// helper functions: union Find for merging sets of nodes
size_t uf_find(size_t *parent, size_t x);
bool uf_union(size_t *parent, size_t *size, size_t x, size_t y);

// a node with 3d coords
typedef int node[3];

// list of nodes
struct nodelist {
  size_t length;
  node *content;
};

long long calc(size_t size, char *buffer, bool to_the_end) {
  size_t line_count = 1;
  int pos = 0;
  // count lines of input text
  while (buffer[pos] != '\0' && pos <= (int)size) {
    if (buffer[pos] == '\n') {
      line_count++;
    }
    pos++;
  }

  size_t node_count = line_count;
  size_t edge_count = node_count * node_count;
  struct nodelist nodes = {};
  nodes.length = node_count;
  nodes.content = malloc(node_count * sizeof(nodes));
  int c = 0;
  // parse each line into a node struct
  for (size_t l = 0; l < line_count; l++) {
    for (size_t d = 0; d < 3; d++) {
      int accum = 0;
      while (buffer[c] != '\0' && c <= (int)size && buffer[c] != '\n' &&
             buffer[c] != ',') {
        accum = accum * 10 + (buffer[c] - '0');
        c++;
      }
      nodes.content[l][d] = accum;
      c++;
    }
  }

  // create list of edge ids
  size_t *edge_ids = malloc(node_count * node_count * sizeof(edge_ids[0]));
  for (size_t e = 0; e < edge_count; e++) {
    edge_ids[e] = e;
  }
  // sort edge ids by their length
  qsort_r(edge_ids, edge_count, sizeof(edge_ids[0]), &nodes,
          &compare_edge_length);

  // initialize union find
  size_t *union_find_parent = malloc(node_count * sizeof(size_t));
  size_t *union_find_size = malloc(node_count * sizeof(size_t));

  for (size_t u = 0; u < node_count; u++) {
    union_find_parent[u] = u;
    union_find_size[u] = 1;
  }

  // for test case only process the first 10 edges 1000 for the real task
  int remaining_trys = node_count < 50 ? 10 : 1000;

  for (size_t e = 0; e < edge_count; e++) {
    size_t edge_id = edge_ids[e];
    size_t n1 = edge_id % node_count;
    size_t n2 = edge_id / node_count;
    // for part 1 break early
    if (!to_the_end && remaining_trys <= 0) {
      break;
    }
    // Skip reflexive Edges
    if (n1 >= n2) {
      continue;
    }

    remaining_trys--;

    DEBUG("%d, try connect %lu to %lu\n", remaining_trys, n1, n2);
    if (!uf_union(union_find_parent, union_find_size, n1, n2)) {
      DEBUG("already connected\n");
    }

    // check if graph is fully connected now
    if (to_the_end) {
      if (union_find_size[uf_find(union_find_parent, n1)] == node_count) {
        return nodes.content[n1][0] * nodes.content[n2][0];
      }
    }
  }

  // for part one multiply the size of the 3 largest components
  size_t product = 1;
  size_t prev_max = SIZE_MAX;
  size_t max = 0;
  for (size_t m = 0; m < 3; m++) {
    for (size_t u = 0; u < node_count; u++) {
      if (u != uf_find(union_find_parent, u)) {
        continue;
      }

      if (max < union_find_size[u] &&
          (m == 0 || union_find_size[u] < prev_max)) {
        max = union_find_size[u];
      }
    }
    prev_max = max;
    product *= max;
  }

  return product;
}
int compare_edge_length(void *n, const void *a, const void *b) {
  struct nodelist nodes = *(struct nodelist *)n;
  long long l1 = 0;
  long long l2 = 0;
  size_t e1 = *(size_t *)a;
  size_t e2 = *(size_t *)b;
  {
    size_t n1 = e1 % nodes.length;
    size_t n2 = e1 / nodes.length;
    for (int d = 0; d < 3; d++) {
      long long sub = nodes.content[n1][d] - nodes.content[n2][d];
      l1 += sub * sub;
    }
  }
  {
    size_t n1 = e2 % nodes.length;
    size_t n2 = e2 / nodes.length;
    for (int d = 0; d < 3; d++) {
      long long sub = nodes.content[n1][d] - nodes.content[n2][d];
      l2 += sub * sub;
    }
  }

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
