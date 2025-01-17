#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "arrange.h"
// #include <signal.h>

// static volatile int keep_running = 1;

// void intHandler(int dummy) { keep_running = 0; }

int main() {

    // signal(SIGINT, intHandler);
    long seed = time(NULL);
    srand(seed);
    printf("Seed: %li\n", seed);
    // srand(1);
    int num_nodes = rand() % 8 + 5;
    int num_edges = rand() % 10 + 8;
    // int num_edges = 1;
    struct Edge *edges = malloc(num_edges * sizeof(*edges));
    for (int i = 0; i < num_edges; i++) {
        int edge_start = rand() % num_nodes;

        // trick: never have edge_end = edge_start, add one if it is greater or equal
        int edge_end = rand() % (num_nodes - 1);
        if (edge_end >= edge_start) {
            edge_end += 1;
        }

        // int weight = rand() % 100;
        edges[i] = (struct Edge) {.start = edge_start, .end = edge_end };
    }
    struct Node *nodes = malloc(num_nodes * sizeof(*nodes));
    arrange_nodes(num_nodes, nodes, num_edges, edges);
    render(num_nodes, nodes, num_edges, edges);
    free(edges);
    free(nodes);
    return 0;
}
