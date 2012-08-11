#ifndef nodeFUCK
#define nodeFUCK

#include "consts.h"

#define NODE_ARRAY_SIZE_FACTOR 2

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

bool add_node(int index, char* name);
bool add_edge(int id1, int id2, double weight, int* countEdges, double* totalWeights);
bool remove_edge(node* nodes, int id1, int id2, int* countEdges, double* totalWeights);
bool add_one_edge(node* nodeFrom, node* nodeTo, double weight);
bool remove_one_edge(node* nodeFrom, node* nodeTo, double* removedWeight);

#endif
