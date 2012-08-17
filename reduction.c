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

bool lp_rhs_sense(int k, double** rhs, char** sense) {
	extern node* nodes;
	extern int nodesCount;
	extern int edgesCount;

	int i;

	/* constraint 1 */
	for (i=0; i<(2*edgesCount*k); i++) {
		rhs[i] = 0;
		sense[i] = 'L';
	}

	/* constraint 2 */
	for (; i<(3*edgesCount*k); i++) {
		rhs[i] = -1;
		sense[i] = 'G';
	}

	/* constraint 3 */
	for (; i<(3*edgesCount*k + nodesCount); i++) {
		rhs[i] = 1;
		sense[i] = 'E';
	}

	/* contraint 4 */
	for (; i<(3*edgesCount*k + nodesCount + k); i++) {
		rhs[i] = 1;
		sense[i] = 'G';
	}

	return TRUE;
}

bool lp_matrix(int k, int *matbeg, int *matcnt, int *matind, double *matval) {
	extern node* nodes;
	extern int nodesCount;
	extern int edgesCount;

	int currRow = 0;

	int i;
	int cluster;
	int nodeIndex;
	edge* currEdge;

	/* constraint 1 */
	for (cluster=0; cluster<k; cluster++) {
		for (nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++) {
			currEdge = nodes[nodeIndex].edges;
			while (currEdge != NULL) {

				matavl.push(-1);
				matind.push(currRow);


				currEdge = currEdge->next;
				currRow++;
			}
		}
	}

	/* constraint 2 */

	/* constraint 3 */

	/* constraint 4 */
}
