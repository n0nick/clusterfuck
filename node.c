#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "consts.h"

bool add_node(int index, char* name) {
	extern node* nodes;

	nodes[index].id = index;
	nodes[index].degree = 0;
	nodes[index].edges = NULL;
	nodes[index].name = (char *) malloc(strlen(name) + 1);
	if (nodes[index].name == NULL) {
		perror(ERROR_MALLOC);
		return FALSE;
	}
	strcpy(nodes[index].name, name);

	return TRUE;
}

bool add_edge(int id1, int id2, double weight) {
	extern node* nodes;

	bool valid;
	node* v1;
	node* v2;
	edge* currEdge;
	int printf_result = 0;
	bool success = TRUE;

	valid = TRUE;

	v1 = &(nodes[id1]);
	v2 = &(nodes[id2]);

	/* check to see if node already exists */
	if (v1->degree < v2->degree) {
		currEdge = v1->edges;
	} else {
		currEdge = v2->edges;
	}
	while (currEdge != NULL && printf_result >= 0) {
		if ((currEdge->nodeTo == v1->id) || (currEdge->nodeTo == v2->id)) {
			printf_result = printf("Error: edge is in the graph\n");
			valid = FALSE;
		}
		currEdge = currEdge->next;
	}

	if (printf_result >= 0) {
		if (valid == TRUE) {
			success = add_one_edge(v1, v2, weight);
			if (success) {
				success = add_one_edge(v2, v1, weight);
				if (success) {
					/* count edge */
					v1->degree++;
					v2->degree++;
				}
			}
		}
	} else {
		perror(ERROR_PRINTF);
		success = FALSE;
	}

	return success;
}

bool add_one_edge(node* nodeFrom, node* nodeTo, double weight) {
	extern edge* edges;

	static int edgesAdded = 0;

	edge* currEdge;
	edge* newEdge;

	/* build new edge */
	newEdge = (edge*) malloc(sizeof(edge));
	if (newEdge != NULL) {
		newEdge->nodeFrom = nodeFrom->id;
		newEdge->nodeTo = nodeTo->id;
		newEdge->weight = weight;
		newEdge->prev = NULL;

		currEdge = nodeFrom->edges;
		if (currEdge == NULL) { /* first edge */
			newEdge->next = NULL;
			nodeFrom->edges = newEdge;
		} else { /* attach to edges list */
			newEdge->next = currEdge;
			currEdge->prev = newEdge;
			nodeFrom->edges = newEdge;
		}
	} else {
		perror(ERROR_MALLOC);
		return FALSE;
	}

	/* add edge to array (only once) */
	if (newEdge->nodeTo < newEdge->nodeFrom) {
		edges[edgesAdded++] = *newEdge;
	}
	return TRUE;
}

bool remove_edge(node* nodes, int id1, int id2) {
	bool removed;
	double removedWeight;
	node* v1;
	node* v2;

	v1 = &(nodes[id1]);
	v2 = &(nodes[id2]);

	removed = ((remove_one_edge(v1, v2, &removedWeight)) && (remove_one_edge(
			v2, v1, &removedWeight)));

	if (!removed) {
		if (printf("Error: edge is not in the graph\n") < 0) {
			perror(ERROR_PRINTF);
			return FALSE;
		}
	} else {
		/* count edge */
		v1->degree--;
		v2->degree--;
	}

	return TRUE;
}

int remove_one_edge(node* nodeFrom, node* nodeTo, double* removedWeight) {
	bool success;
	edge* currEdge;
	success = FALSE;

	currEdge = nodeFrom->edges;
	while ((currEdge != NULL) && (!success)) {
		if (currEdge->nodeTo == nodeTo->id) {

			/* first */
			if (currEdge->prev == NULL) {
				nodeFrom->edges = currEdge->next;
				if (currEdge->next != NULL) { /* first but not last */
					currEdge->next->prev = NULL;
				}
			}

			/* last */
			else if (currEdge->next == NULL) {
				currEdge->prev->next = NULL;
			}

			/* middle */
			else {
				currEdge->prev->next = currEdge->next;
				currEdge->next->prev = currEdge->prev;
			}

			success = TRUE;
		}

		if (!success) {
			currEdge = currEdge->next;
		}
	}

	if (success) {
		*removedWeight = currEdge->weight;
		free(currEdge);
	}

	return (success);
}

bool lookup_node(char* name, int* idx) {
	extern node* nodes;
	extern int nodesCount;
	int i;

	for (i = 0; i < nodesCount; i++) {
		if (strcmp(name, (nodes[i]).name) == 0) {
			*idx = i;
			return TRUE;
		}
	}

	return FALSE;
}

bool print_nodes() {
	extern node* nodes;
	extern int nodesCount;
	int i;
	int printf_result;

	printf_result = printf("%d nodes:\n", nodesCount);

	for (i = 0; i < nodesCount && printf_result >= 0; i++) {
		printf_result = printf("%d: %s\n", nodes[i].id, nodes[i].name);
	}

	if (printf_result < 0) {
		perror(ERROR_PRINTF);
		return FALSE;
	}
	return TRUE;
}

bool print_edges() {
	extern node* nodes;
	extern int nodesCount;
	int i;
	edge* currEdge;
	int printf_result;

	bool success = TRUE;

	printf_result = printf("edges:\n");

	for (i = 0; i < nodesCount && printf_result >= 0; i++) {
		currEdge = nodes[i].edges;
		while (currEdge != NULL && printf_result >= 0) {
			if (i < currEdge->nodeTo) { /* to conform with supplied tests */
				printf_result = printf("%s %s %f\n",
						nodes[currEdge->nodeTo].name,
						nodes[currEdge->nodeFrom].name, currEdge->weight);
			}
			currEdge = currEdge->next;
		}
	}

	if (printf_result < 0) {
		perror(ERROR_PRINTF);
		success = FALSE;
	}

	return success;
}
