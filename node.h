#ifndef nodeFUCK
#define nodeFUCK

#define VERTEX_ARRAY_INITIAL_SIZE 2
#define NODE_ARRAY_SIZE_FACTOR 2

#define MAX_nodes 100
#define MAX_WEIGHT 100
#define MAX_TOTAL_WEIGHTS 1000

typedef struct edge edge;
typedef struct node node;

struct edge {
	unsigned int nodeID;
	double weight;
	edge* next;
	edge* prev;
};

struct node {
	unsigned int id;
	char* name;
	unsigned int degree;
	edge* edges;
};

node* add_node(node* currentArray, int* size, int* maxSize, char* name, int* success);
void print_nodes(node* nodes, int size, int* success);
void print_edges(node* nodes, int size, int* success);
void print_degree(node* nodes, int id, int* success);
void print_by_name(node* nodes, char* name, int size, int* success);

void add_edge(node* nodes, int id1, int id2, double weight, int* countEdges, double* totalWeights, int* success);
void remove_edge(node* nodes, int id1, int id2, int* countEdges, double* totalWeights, int* success);
void add_one_edge(node* nodeFrom, node* nodeTo, double weight, int* success);
int remove_one_edge(node* nodeFrom, node* nodeTo, double* removedWeight);

#endif
