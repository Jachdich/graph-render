
struct Edge {
    char start;
    char end;
};

struct Node {
    int id;
    double x;
    double y;
};
void render(int num_nodes, struct Node *nodes, int num_edges, struct Edge *edges);
void arrange_nodes(int num_nodes, struct Node *nodes, int num_edges, struct Edge *edges);
