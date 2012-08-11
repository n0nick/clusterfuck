#ifndef superCLUSTERagilisticexpialidociouVERTEX
#define superCLUSTERagilisticexpialidociouVERTEX

#define VERTEX_ARRAY_INITIAL_SIZE 2
#define VERTEX_ARRAY_SIZE_FACTOR 2

#define MAX_VERTICES 100
#define MAX_WEIGHT 100
#define MAX_TOTAL_WEIGHTS 1000

typedef struct edge edge;
typedef struct vertex node;

struct edge {
	unsigned int vertexID;
	double weight;
	edge* next;
	edge* prev;
};

struct vertex {
	unsigned int id;
	char* name;
	unsigned int degree;
	edge* edges;
};

node* add_vertex(node* currentArray, int* size, int* maxSize, char* name, int* success);
void print_vertices(node* vertices, int size, int* success);
void print_edges(node* vertices, int size, int* success);
void print_degree(node* vertices, int id, int* success);
void print_by_name(node* vertices, char* name, int size, int* success);

void add_edge(node* vertices, int id1, int id2, double weight, int* countEdges, double* totalWeights, int* success);
void remove_edge(node* vertices, int id1, int id2, int* countEdges, double* totalWeights, int* success);
void add_one_edge(node* vertexFrom, node* vertexTo, double weight, int* success);
int remove_one_edge(node* vertexFrom, node* vertexTo, double* removedWeight);

#endif
