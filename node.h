#ifndef nodeFUCK
#define nodeFUCK

#include "consts.h"

#define NODE_ARRAY_SIZE_FACTOR 2

typedef struct edge edge;
typedef struct node node;

struct edge {
	unsigned int nodeFrom;
	unsigned int nodeTo;
	double weight;
	edge* next;
	edge* prev;
};

struct node {
	unsigned int id;
	char* name;
	unsigned int degree;
	int clusterID;
	edge* edges;
};

bool add_node(int index, char* name);
bool add_edge(int id1, int id2, double weight);
bool remove_edge(node* nodes, int id1, int id2);
bool add_one_edge(node* nodeFrom, node* nodeTo, double weight);
bool remove_one_edge(node* nodeFrom, node* nodeTo, double* removedWeight);
bool lookup_node(char* name, int* idx);
bool print_nodes();
bool print_edges();

#endif
