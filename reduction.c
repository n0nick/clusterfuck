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

/* debug */
#include <stdio.h>

bool lp_objective_function_coefficients(int k, double** coefficients) {
	extern node* nodes;
	extern int nodesCount;
	extern int edgesCount;
	int index = 0;
	int i, cluster;
	edge* currEdge;

	int numcols = k * (nodesCount + edgesCount);

	coefficients = (double**) malloc(sizeof(double*) * numcols);
	for (i = 0; i < numcols; i++) {
		coefficients[i] = (double*) malloc(sizeof(double));
	}

	/* X's coefficients are all 0s. */
	for (cluster = 0; cluster < k; cluster++) {
		for(i = 0; i < nodesCount; i++) {
			*(coefficients[index++]) = 0;
		}
	}

	/* Z's coefficients are the edges' weights */
	for (cluster = 0; cluster < k; cluster++) {
		for (i = 0; i < nodesCount; i++) {
			currEdge = nodes[i].edges;
			while (currEdge != NULL) {
				if(currEdge->nodeFrom < currEdge->nodeTo) {
					*(coefficients[index++]) = currEdge->weight;
				}
				currEdge = currEdge->next;
			}
		}
	}

	return TRUE;
}

bool lp_rhs_sense(int k, double** rhs, char** sense) {
	extern node* nodes;
	extern int nodesCount;
	extern int edgesCount;

	int i;
	int size = (3*edgesCount*k + nodesCount + k);

	rhs = (double**) malloc(sizeof(double*) * size);
	sense = (char**) malloc(sizeof(char*) * size);
	for (i=0; i<size; i++) {
		rhs[i] = (double*) malloc(sizeof(double));
		sense[i] = (char*) malloc(sizeof(char));
	}

	/* constraint 1 */
	for (i=0; i<(2*edgesCount*k); i++) {
		*rhs[i] = 0;
		*sense[i] = 'L';
	}

	/* constraint 2 */
	for (; i<(3*edgesCount*k); i++) {
		*rhs[i] = -1;
		*sense[i] = 'G';
	}

	/* constraint 3 */
	for (; i<(3*edgesCount*k + nodesCount); i++) {
		*rhs[i] = 1;
		*sense[i] = 'E';
	}

	/* contraint 4 */
	for (; i<(3*edgesCount*k + nodesCount + k); i++) {
		*rhs[i] = 1;
		*sense[i] = 'G';
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
	int edgeIndex = 0;
	edge* currEdge;

	/* constraint 1 */
	for (cluster=0; cluster<k; cluster++) {
		for (nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++) {
			currEdge = nodes[nodeIndex].edges;
			while (currEdge != NULL) {

				col = k*|V| + k*|E| + e;

				/* putting Z's */
				if (matbeg[col] == 0)
					x = ...
					matbeg[col] = x
				else
					x = matbeg[col] + matcnt[col] + 1

				matcnt[col]++;

				matval[x] = 1;
				matind[x] = lineIndex;

				/* putting X's */



				currEdge = currEdge->next;
				currRow++;
				edgeIndex++;
			}
		}
	}

	/* constraint 2 */

	/* constraint 3 */

	/* constraint 4 */
}
