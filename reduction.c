/*
 * reduction.c
 *
 *  Created on: Aug 17, 2012
 *      Author: amirmoua
 */

#include <stdlib.h>
#include "reduction.h"
#include "consts.h"
#include "node.h"

#include <stdio.h>

bool lp_objective_function_coefficients(int k, double** coefficients) {
	extern node* nodes;
	extern int nodesCount;
	extern int edgesCount;
	int i, j;
	edge* currEdge;

	int numcols = k * (nodesCount + edgesCount);

	coefficients = (double**) malloc(sizeof(double*) * numcols);
	for (i = 0; i < numcols; i++) {
		coefficients[i] = (double*) malloc(sizeof(double));
	}

	/* X's coefficients are all 0s. */
	for(i = 0; i < (k * nodesCount); i++) {
		*(coefficients[i]) = 0;
	}

	/* Z's coefficients are the edges' weights */
	for (j = 0; j < nodesCount; j++) {
		currEdge = nodes[j].edges;
		while (currEdge != NULL) {
			*(coefficients[i++]) = currEdge->weight;
			currEdge = currEdge->next;
		}
	}

	/* debug */
	for (i=0; i<numcols; i++) {
		printf("%f, ", *(coefficients[i]));
	}

	return TRUE;
}
