#include "day.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define DIMS 3
#define MAXS 3

long long calc(size_t size, char *buffer, bool to_the_end);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }

// helper function: compare edge ids by their edges length
int compare_edge_length(const void *a, const void *b, void *l);

// helper functions: union Find for merging sets of nodes
size_t uf_find(size_t *parent, size_t x);
bool uf_union(size_t *parent, size_t *size, size_t x, size_t y);

// a node with 3d coords
typedef long long node[DIMS];

// list of nodes
struct nodelist {
  size_t length;
  node *content;
};
// list of nodes
struct edgelist {
  size_t length;
  size_t *edge_ids;
  size_t *union_find_parent;
  size_t *union_find_size;
};

// structure to store nodes and edges
// and union find data
struct graph {
  struct nodelist nodes;
  struct edgelist edges;
};

// allocate memory for graph in one chunk
void alloc_graph(struct graph *g, size_t node_count) {
  size_t edge_count = node_count * node_count;
  size_t *graph_storage =
      malloc(node_count * (node_count + 2) * node_count * sizeof(size_t) +
             node_count * sizeof(node));
  size_t *edge_storage = graph_storage;
  size_t *node_storage = graph_storage + node_count * (2 * node_count);

  g->nodes.length = node_count;
  g->nodes.content = (node *)node_storage;
  g->edges.edge_ids = edge_storage + 2 * node_count;
  g->edges.union_find_parent = edge_storage + node_count;
  g->edges.union_find_size = edge_storage;

  // initialize edge ids
  for (size_t e = 0; e < edge_count; e++) {
    g->edges.edge_ids[e] = e;
  }

  // init union find
  for (size_t u = 0; u < node_count; u++) {
    g->edges.union_find_parent[u] = u;
    g->edges.union_find_size[u] = 1;
  }
}

long long calc(size_t size, char *buffer, bool to_the_end) {
  size_t node_count = 1;
  {
    // count lines of input text
    int pos = 0;
    while (buffer[pos] != '\0' && pos <= (int)size) {
      if (buffer[pos] == '\n') {
        node_count++;
      }
      pos++;
    }
  }

  size_t edge_count = node_count * node_count;

  // init graph
  struct graph g = {};
  alloc_graph(&g, node_count);

  {
    // parse each line into a node struct
    int c = 0;
    for (size_t l = 0; l < node_count; l++) {
      for (size_t d = 0; d < DIMS; d++) {
        int accum = 0;
        while (buffer[c] != '\0' && c <= (int)size && buffer[c] != '\n' &&
               buffer[c] != ',') {
          accum = accum * 10 + (buffer[c] - '0');
          c++;
        }
        g.nodes.content[l][d] = accum;
        c++;
      }
    }
  }

  // sort edge ids by their length
  qsort_r(g.edges.edge_ids, edge_count, sizeof(g.edges.edge_ids[0]),
          &compare_edge_length, &g.nodes);

  // for test case only process the first 10 edges 1000 for the real task
  int remaining_trys = node_count < 50 ? 10 : 1000;

  for (size_t e = 0; e < edge_count; e++) {
    // convert edge index to source and target ode
    size_t edge_id = g.edges.edge_ids[e];
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

    // try to connect components
    uf_union(g.edges.union_find_parent, g.edges.union_find_size, n1, n2);

    // for part 2: stop if graph is fully connected now
    if (g.edges.union_find_size[uf_find(g.edges.union_find_parent, n1)] ==
        node_count) {
      // calculate solution value for part 2 as product of x coords
      return g.nodes.content[n1][0] * g.nodes.content[n2][0];
    }
  }

  // for part one multiply the size of the 3 largest components
  long long product = 1;
  long long prev_max = 0;
  for (size_t m = 0; m < MAXS; m++) {
    long max = 0;
    for (size_t u = 0; u < node_count; u++) {
      if (u != uf_find(g.edges.union_find_parent, u)) {
        continue;
      }

      if (max < (long long)g.edges.union_find_size[u] &&
          (m == 0 || ((long long)g.edges.union_find_size[u]) < prev_max)) {
        max = (long long)g.edges.union_find_size[u];
      }
    }
    prev_max = max;
    product *= max;
  }

  return product;
}

// length of edge e by L2 dist between nodes
long long euclid(size_t e, struct nodelist nodes) {
  long long l = 0;
  size_t n1 = e % nodes.length;
  size_t n2 = e / nodes.length;
  for (int d = 0; d < DIMS; d++) {
    long long sub = nodes.content[n1][d] - nodes.content[n2][d];
    l += sub * sub;
  }
  return l;
}

// compare two edges by their length
int compare_edge_length(const void *a, const void *b, void *n) {
  struct nodelist nodes = *(struct nodelist *)n;
  size_t e1 = *(size_t *)a;
  size_t e2 = *(size_t *)b;
  long long l1 = euclid(e1, nodes);
  long long l2 = euclid(e2, nodes);

  return (l1 > l2) - (l1 < l2);
}

// find topmost parent of x in its component
size_t uf_find(size_t *parent, size_t x) {
  if (parent[x] != x)
    parent[x] = uf_find(parent, parent[x]);
  return parent[x];
}

// merge node x and y if not already in same component
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
