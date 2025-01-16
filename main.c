#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

struct Edge {
    char start;
    char end;
    int weight;
};

struct Node {
    int id;
    double x;
    double y;
};

void draw_line(char *buffer, int w, int h, int x1, int y1, int x2, int y2) {

    float m = (float)(y2-y1) / (float)(x2-x1);
    if (fabs(m) < 1) {
        if (x1 > x2) {
            int x = x1;
            int y = y1;
            x1 = x2;
            x2 = x;
            y1 = y2;
            y2 = y;
        }
        m = (float)(y2-y1) / (float)(x2-x1);
        for (int x = x1; x < x2; x++) {
            float y = (float)y1 + (x - x1) * m;
            buffer[((int)y) * w + (int)x] = 'o';
        }
    } else {
        if (y1 > y2) {
            int x = x1;
            int y = y1;
            x1 = x2;
            x2 = x;
            y1 = y2;
            y2 = y;
        }
        m = (float)(x2-x1) / (float)(y2-y1);
        for (int y = y1; y < y2; y++) {
            float x = (float)x1 + (y - y1) * m;
            buffer[((int)y) * w + (int)x] = 'o';
        }
    }
}

void render(int num_nodes, struct Node *nodes, int num_edges, struct Edge *edges) {
    const int w = 128;
    const int h = 60; // could be 64...
    char buffer[w * h];
    memset(buffer, ' ', w * h);
    float min_x = 1000000;
    float min_y = 1000000;
    float max_x = -1000000;
    float max_y = -1000000;
    for (int i = 0; i < num_nodes; i++) {
        if (nodes[i].x < min_x) min_x = nodes[i].x;
        if (nodes[i].x > max_x) max_x = nodes[i].x;
        if (nodes[i].y < min_y) min_y = nodes[i].y;
        if (nodes[i].y > max_y) max_y = nodes[i].y;
    }
    float scl_x = (float)(w - 8) / (max_x - min_x);
    float scl_y = (float)(h - 8) / (max_y - min_y);
    float scl = fmin(scl_x, scl_y);
    scl_x = scl;
    scl_y = scl;
    float offset_x = -min_x * scl_x + 3;
    float offset_y = -min_y * scl_y + 3;
    
    for (int i = 0; i < num_edges; i++) {
        struct Node n1 = nodes[edges[i].start];
        struct Node n2 = nodes[edges[i].end];
        draw_line(buffer, w, h, n1.x * scl_x + offset_x,
                                n1.y * scl_y + offset_y,
                                n2.x * scl_x + offset_x,
                                n2.y * scl_y + offset_y);
    }

    for (int i = 0; i < num_nodes; i++) {
        float cx = nodes[i].x * scl_x + offset_x;
        float cy = nodes[i].y * scl_y + offset_y;
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++) {
                buffer[((int)cy + dy) * w + (int)cx + dx] = '#';
            }
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            putchar(buffer[y * w + x]);
        }
        putchar('\n');
    }
}

void repell(int num_nodes, struct Node *nodes) {
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (i == j) continue;
            float dx = nodes[i].x - nodes[j].x;
            float dy = nodes[i].y - nodes[j].y;
            float mag2 = dx * dx + dy * dy;

            // hack: if the mag is close to 0, it may cause dx and dy to become nan
            if (mag2 < 0.0000000001) {
                continue;
            }
            float force = 0.3 / mag2;
            float mag = sqrt(mag2);
            dx = dx / mag * force; // TODO this can be simplified
            dy = dy / mag * force;
            nodes[i].x += dx;
            nodes[i].y += dy;
        }
    }
}

void attract(int num_nodes, struct Node *nodes, int num_edges, struct Edge *edges) {
    for (int i = 0; i < num_edges; i++) {
        int a = edges[i].start;
        int b = edges[i].end;
        float dx = nodes[a].x - nodes[b].x;
        float dy = nodes[a].y - nodes[b].y;
        float mag = sqrt(dx * dx + dy * dy);
        // hack: if the mag is close to 0, it may cause dx and dy to become nan
        if (mag < 0.0000000001) {
            continue;
        }
        dx = dx / mag * 0.07;
        dy = dy / mag * 0.07;
        nodes[a].x -= dx;
        nodes[a].y -= dy;
        nodes[b].x += dx;
        nodes[b].y += dy;
    }

    float avg_x = 0;
    float avg_y = 0;
    for (int i = 0; i < num_nodes; i++) {
        avg_x += nodes[i].x / num_nodes;
        avg_y += nodes[i].y / num_nodes;
    }
    for (int i = 0; i < num_nodes; i++) {
        float dx = avg_x - nodes[i].x;
        float dy = avg_y - nodes[i].y;
        float mag2 = dx * dx + dy * dy;
        float force = mag2 > 20 ? 1 : 0;
        float mag = sqrt(mag2);
        dx = dx / mag * force; // TODO this can be simplified
        dy = dy / mag * force;
        nodes[i].x += dx;
        nodes[i].y += dy;
    }
}

void display_graph(int num_nodes, int num_edges, struct Edge *edges) {
    struct Node *nodes = malloc(num_nodes * sizeof(*nodes));
    int w = (int)ceil(sqrt((double)num_nodes)); 
    for (int i = 0; i < num_nodes; i++) {
        nodes[i] = (struct Node) {.id = i, .x = i % w, .y = i / w};
    }

    render(num_nodes, nodes, num_edges, edges);

    for (int i = 0; i < 10000; i++) {
        repell(num_nodes, nodes);
        attract(num_nodes, nodes, num_edges, edges);
        render(num_nodes, nodes, num_edges, edges);
        usleep(10000);
    }
    render(num_nodes, nodes, num_edges, edges);
}

int main() {
    srand(time(NULL));
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

        int weight = rand() % 100;
        edges[i] = (struct Edge) {.start = edge_start, .end = edge_end, .weight = weight};
    }
    display_graph(num_nodes, num_edges, edges);
    free(edges);
    return 0;
}
