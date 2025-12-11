#include "day.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define DEBUG(...) {};
// #define DEBUG(...) printf(__VA_ARGS__);

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

long long calc(size_t size, char *buffer, bool part2);

long long part1(size_t size, char *buffer) { return calc(size, buffer, false); }
long long part2(size_t size, char *buffer) { return calc(size, buffer, true); }
long count_paths(size_t node_count, bool matrix[node_count][node_count],
                 size_t topo_sorted[node_count], size_t start, size_t end);
long long calc(size_t size, char *buffer, bool part2) {
  // count number of lines
  size_t node_count = 1;
  {
    // count total lines
    size_t pos = 0;
    while (buffer[pos] != '\0' && pos <= size) {
      if (buffer[pos] == '\n') {
        node_count++;
      }
      pos++;
    }
  }
  node_count++;

  // indices for nodes we are interested in
  size_t you_node = 0;
  size_t out_node = node_count - 1;
  size_t fft_node = 0;
  size_t dac_node = 0;
  size_t svr_node = 0;

  // allocate adjecency matrix
  bool matrix[node_count][node_count] = {};
  // the number of incoming edges for each node
  long indegree[node_count] = {};
  // the topologically sorted indices of the nodes
  size_t topo_sorted[node_count] = {};

  // queue for toposort
  long queue[node_count] = {};
  size_t queue_head = 0;
  size_t queue_tail = 0;

  // parse edges
  {
    size_t source_index = 0;
    size_t pos = 0;
    while (buffer[pos] != '\0' && pos <= size) {
      // at the start of the line, check the label
      // to see if its one the nodes we are interested in
      if (strncmp(&buffer[pos], "you:", 4) == 0) {
        you_node = source_index;
      } else if (strncmp(&buffer[pos], "dac:", 4) == 0) {
        dac_node = source_index;
      } else if (strncmp(&buffer[pos], "fft:", 4) == 0) {
        fft_node = source_index;
      } else if (strncmp(&buffer[pos], "svr:", 4) == 0) {
        svr_node = source_index;
      }
      // skip top double colon
      while (buffer[pos] != ':') {
        pos++;
      }
      pos++;

      // remaining read adjecent node labels
      while (buffer[pos] == ' ') {
        pos++;
        size_t label_start = pos;
        size_t label_end = pos;
        // read label to the next space or line break
        while (buffer[pos] != ' ' && buffer[pos] != '\n' && pos < size) {
          pos++;
          label_end++;
        }

        // find node index (line number) for the label
        size_t target_index = 0;
        {
          // count total lines
          size_t p = 0;
          while (buffer[p] != '\0' && p <= size) {
            if (strncmp(&buffer[p], &buffer[label_start],
                        label_end - label_start) == 0) {
              break;
            }
            while (buffer[p++] != '\n' && p <= size)
              ;
            target_index++;
          }
        }

        // now we know the index of the target node
        // add entry to adjecency matrix and increment indegree
        // of target node
        matrix[source_index][target_index] = 1;
        indegree[target_index] += 1;
      }
      pos++;
      source_index++;
    }
  }

  // topologically sort all nodes
  // all all nodes with no incoming edges
  // to the queue
  for (size_t n = 0; n < node_count; n++) {
    if (indegree[n] == 0) {
      queue[queue_tail++] = n;
    }
  }

  // take node from the queue and remove
  // it from the graph by decrementing its
  // neighbors edge degrees
  size_t rank = 0;
  while (queue_head < queue_tail) {
    size_t node = queue[queue_head++];
    // assign the sort position to the processed node
    topo_sorted[rank++] = node;
    for (size_t w = 0; w < node_count; w++) {
      if (matrix[node][w]) {
        indegree[w]--;

        // add neighbor to the queue if its own
        // degree is zero now
        if (indegree[w] == 0)
          queue[queue_tail++] = w;
      }
    }
  }

  // Debug output
  DEBUG("Node Count: %lu \n", node_count);
  DEBUG("You Node: %lu \n", you_node);
  DEBUG("Out Node: %lu \n", out_node);
  DEBUG("Degrees: \n");
  for (size_t y = 0; y < node_count; y++) {
    DEBUG("%lu", indegree[y]);
  }
  DEBUG("\n");
  DEBUG("Sorted: \n");
  for (size_t y = 0; y < node_count; y++) {
    DEBUG("%lu", topo_sorted[y]);
  }
  DEBUG("\n");
  DEBUG("Matrix: \n");
  for (size_t y = 0; y < node_count; y++) {
    for (size_t x = 0; x < node_count; x++) {
      DEBUG("%b", matrix[y][x]);
    }
    DEBUG("\n");
  }

  // for part 1 count the paths from you to out
  if (!part2) {
    return count_paths(node_count, matrix, topo_sorted, you_node, out_node);
  } else {
    // for part 2 count the paths between each waypoint seperately
    long svr_to_fft =
        count_paths(node_count, matrix, topo_sorted, svr_node, fft_node);
    long svr_to_dac =
        count_paths(node_count, matrix, topo_sorted, svr_node, dac_node);
    long fft_to_dac =
        count_paths(node_count, matrix, topo_sorted, fft_node, dac_node);
    long dac_to_fft =
        count_paths(node_count, matrix, topo_sorted, dac_node, fft_node);
    long dac_to_out =
        count_paths(node_count, matrix, topo_sorted, dac_node, out_node);
    long fft_to_out =
        count_paths(node_count, matrix, topo_sorted, fft_node, out_node);

    // then multiply and some for total number
    // of combinations
    return (svr_to_fft * fft_to_dac * dac_to_out) +
           (svr_to_dac * dac_to_fft * fft_to_out);
  }
}

// based on the topologically sorted graph
// we can count the number of paths via dynamic programming
long count_paths(size_t node_count, bool matrix[node_count][node_count],
                 size_t topo_sorted[node_count], size_t start, size_t end) {
  long path_counts[node_count] = {};
  path_counts[start] = 1;
  for (size_t i = 0; i < node_count; i++) {
    // starting add the nodes with no outgoing edges
    int v = topo_sorted[i];
    if (path_counts[v] == 0)
      continue;
    // we add their own path counts
    // to all their neighbors
    for (size_t w = 0; w < node_count; w++) {
      if (matrix[v][w])
        path_counts[w] += path_counts[v];
    }
  }
  return path_counts[end];
}
