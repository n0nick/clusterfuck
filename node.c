#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "consts.h"

node* add_node(node* currentArray, int* size, int* maxSize, char* name, int* success) {
	node* newArray;
	int i;
	int failIndex;

	*success = SUCCESS;

	if((*size) >= (*maxSize)) {
		/* re-allocate memory */
		(*maxSize) = (*maxSize) * NODE_ARRAY_SIZE_FACTOR;
		newArray = (node *) malloc((*maxSize) * sizeof(node));
		if (newArray == NULL) {
			perror(ERROR_MALLOC);
			*success = FAILURE;
		}

		/* copy current content */
		for(i = 0; i < (*size) && *success; i++) {
			newArray[i].degree = currentArray[i].degree;
			newArray[i].edges = currentArray[i].edges;
			newArray[i].id = currentArray[i].id;
			newArray[i].name = (char *) malloc(strlen(currentArray[i].name) + 1);
			if (newArray[i].name == NULL) {
				perror(ERROR_MALLOC);
				failIndex = i;
				*success = FAILURE;
			} else {
				strcpy(newArray[i].name, currentArray[i].name);
				free(currentArray[i].name);
			}
		}

		if (*success) {
			/* releasing old memory */
			free(currentArray);
		}
	} else {
		newArray = currentArray;
	}

	if (*success == SUCCESS) {
		/* initializing values */
		newArray[*size].id = *size;
		newArray[*size].degree = 0;
		newArray[*size].edges = NULL;
		newArray[*size].name = (char *) malloc(strlen(name) + 1);
		if (newArray[*size].name == NULL) {
			perror(ERROR_MALLOC);
			failIndex = *size;
			*success = FAILURE;
		}
		strcpy(newArray[*size].name, name);
		(*size)++;

		currentArray = newArray;
	}

	if (*success == FAILURE) {
		if (newArray != NULL) {
			for (i = 0; i < failIndex; i++) {
				free(newArray[i].name);
			}
			free(newArray);
		}
	}

	return currentArray;
}

void print_nodes(node* nodes, int size, int* success) {
	int i;
	int printf_result;

	*success = SUCCESS;

	printf_result = printf("%d nodes:\n", size);

	for(i = 0; i < size && printf_result >= 0; i++) {
		printf_result = printf("%d: %s\n", nodes[i].id, nodes[i].name);
	}

	if (printf_result < 0) {
		perror(ERROR_PRINTF);
		*success = FAILURE;
	}
}

void print_edges(node* nodes, int size, int* success) {
	int i;
	edge* currEdge;
	int printf_result;

	*success = SUCCESS;

	printf_result = printf("edges:\n");

	for(i = 0; i < size && printf_result >= 0; i++) {
		currEdge = nodes[i].edges;
		while(currEdge != NULL && printf_result >= 0) {
			if (i < currEdge->nodeID) { /* to conform with supplied tests */
				printf_result = printf("%s %s %f\n", nodes[currEdge->nodeID].name, nodes[i].name, currEdge->weight);
			}
			currEdge = currEdge->next;
		}
	}

	if (printf_result < 0) {
		perror(ERROR_PRINTF);
		*success = FAILURE;
	}
}

void print_degree(node* nodes, int id, int* success) {
	*success = SUCCESS;

	if (printf("%d\n", nodes[id].degree) < 0) {
		perror(ERROR_PRINTF);
		*success = FAILURE;
	}
}

void print_by_name(node* nodes, char* name, int size, int* success) {
	int i;
	int printf_result = 0;
	bool found = FALSE;

	*success = SUCCESS;

	for (i = 0;  i < size && printf_result >= 0; i++) {
		if (strcmp(nodes[i].name, name) == 0) {
			printf_result = printf("%d\n", nodes[i].id);
			found = TRUE;
		}
	}

	if (printf_result >= 0) {
		if(found == FALSE) {
			printf_result = printf("Error: node name is not in the system\n");
		}
	}

	if (printf_result < 0) {
		perror(ERROR_PRINTF);
		*success = FAILURE;
	}
}

void add_edge(node* nodes, int id1, int id2, double weight, int* countEdges, double* totalWeights, int* success) {

	bool valid;
	node* v1;
	node* v2;
	edge* currEdge;
	int printf_result = 0;

	*success = SUCCESS;
	valid = TRUE;

	/* no edges between the same node */
	if (id1 == id2) {
		printf_result = printf("Error: edge must be between two different nodes\n");
		valid = FALSE;
	}

	if (valid == TRUE && printf_result >= 0) {
		v1 = &(nodes[id1]);
		v2 = &(nodes[id2]);

		/* check to see if node already exists */
		if (v1->degree < v2->degree) {
			currEdge = v1->edges;
		} else {
			currEdge = v2->edges;
		}
		while (currEdge != NULL && printf_result >= 0) {
			if ((currEdge->nodeID == v1->id) ||
				(currEdge->nodeID == v2->id)) {
					printf_result = printf("Error: edge is in the graph\n");
					valid = FALSE;
			}
			currEdge = currEdge->next;
		}
	}

	if (printf_result >= 0) {
		if (valid == TRUE) {
			add_one_edge(v1, v2, weight, success);
			if (*success) {
				add_one_edge(v2, v1, weight, success);
				if (*success) {
					/* count edge */
					*countEdges = *countEdges + 1;
					*totalWeights = *totalWeights + weight;
					v1->degree++;
					v2->degree++;
				}
			}
		}
	} else {
		perror(ERROR_PRINTF);
		*success = FAILURE;
	}
}

void add_one_edge(node* nodeFrom, node* nodeTo, double weight, int* success) {
	edge* currEdge;
	edge* newEdge;

	*success = SUCCESS;

	/* build new edge */
	newEdge = (edge*) malloc(sizeof(edge));
	if (newEdge != NULL) {
		newEdge->nodeID = nodeTo->id;
		newEdge->weight = weight;
		newEdge->prev = NULL;

		currEdge = nodeFrom->edges;
		if(currEdge == NULL) { /* first edge */
			newEdge->next = NULL;
			nodeFrom->edges = newEdge;
		} else { /* attach to edges list */
			newEdge->next = currEdge;
			currEdge->prev = newEdge;
			nodeFrom->edges = newEdge;
		}
	} else {
		perror(ERROR_MALLOC);
		*success = FAILURE;
	}
}

void remove_edge(node* nodes, int id1, int id2, int* countEdges, double* totalWeights, int* success) {
	
	bool removed;
	double removedWeight;
	node* v1;
	node* v2;

	*success = SUCCESS;

	v1 = &(nodes[id1]);
	v2 = &(nodes[id2]);

	removed = ((remove_one_edge(v1, v2, &removedWeight)) && (remove_one_edge(v2, v1, &removedWeight)));

	if(!removed) {
		if (printf("Error: edge is not in the graph\n") < 0) {
			perror(ERROR_PRINTF);
			*success = FAILURE;
		}
	} else {
		/* count edge */
		*countEdges = *countEdges - 1;
		*totalWeights = *totalWeights - removedWeight;
		v1->degree--;
		v2->degree--;
	}
}

int remove_one_edge(node* nodeFrom, node* nodeTo, double* removedWeight) {

	bool didDelete;
	edge* currEdge;
	didDelete = FALSE;

	currEdge = nodeFrom->edges;
	while((currEdge != NULL) && (!didDelete)) {
		if(currEdge->nodeID == nodeTo->id) {

			/* first */
			if(currEdge->prev == NULL) {
				nodeFrom->edges = currEdge->next;
				if(currEdge->next != NULL) { /* first but not last */
					currEdge->next->prev = NULL;
				}
			}

			/* last */
			else if(currEdge->next == NULL) {
				currEdge->prev->next = NULL;
			}

			/* middle */
			else {
				currEdge->prev->next = currEdge->next;
				currEdge->next->prev = currEdge->prev;
			}

			didDelete = TRUE;
		}

		if(!didDelete) {
			currEdge = currEdge->next;
		}
	}

	if(didDelete) {
		*removedWeight = currEdge->weight;
		free(currEdge);
	}

	return (didDelete);
}
