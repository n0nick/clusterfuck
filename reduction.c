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
	int index;
	int i, cluster;
	edge* currEdge;

	int numcols = k * (nodesCount + edgesCount);

	/* allocate memory for coefficients array */
	coefficients = calloc(sizeof(double*), numcols);
	for (i = 0; i < numcols; i++) {
		coefficients[i] = calloc(sizeof(double), 1);
	}

	index = 0;

	/* Z's coefficients are the edges' weights */
	for (cluster = 0; cluster < k; cluster++) {
		for (i = 0; i < nodesCount; i++) {
			currEdge = nodes[i].edges;
			while (currEdge != NULL) {
				if (currEdge->nodeTo < currEdge->nodeFrom) {
					*(coefficients[index++]) = currEdge->weight;
				}
				currEdge = currEdge->next;
			}
		}
	}

	/* X's coefficients are all 0s. */
	/* TODO necessary? */
	for (cluster = 0; cluster < k; cluster++) {
		for (i = 0; i < nodesCount; i++) {
			*(coefficients[index++]) = 0;
		}
	}

	return TRUE;
}

bool lp_rhs_sense(int k, double** rhs, char** sense) {
	extern node* nodes;
	extern int nodesCount;
	extern int edgesCount;

	bool success;
	int i;
	int size = (3 * edgesCount * k + nodesCount + k);

	/* allocate memory for rhs, sense */
	rhs = calloc(sizeof(double*), size);
	sense = calloc(sizeof(char*), size);
	success = (rhs != NULL && sense != NULL);

	if (success) {
		for (i = 0; (i < size) && success; i++) {
			rhs[i] = calloc(sizeof(double), 1);
			sense[i] = calloc(sizeof(char), 1);
			success = (rhs[i] != NULL && sense[i] != NULL);
		}
	}

	if (!success) { /* one of the calloc()s failed */
		return FALSE;
	}

	/* constraint 1 */
	for (i = 0; i < (2 * edgesCount * k); i++) {
		*rhs[i] = 0;
		*sense[i] = 'L';
	}

	/* constraint 2 */
	for (; i < (3 * edgesCount * k); i++) {
		*rhs[i] = -1;
		*sense[i] = 'G';
	}

	/* constraint 3 */
	for (; i < (3 * edgesCount * k + nodesCount); i++) {
		*rhs[i] = 1;
		*sense[i] = 'E';
	}

	/* contraint 4 */
	for (; i < (3 * edgesCount * k + nodesCount + k); i++) {
		*rhs[i] = 1;
		*sense[i] = 'G';
	}

	/* debug

	printf("\nsense:\n");
	for (i=0; i<(3 * edgesCount * k + nodesCount + k); i++) {
		printf("%c", *sense[i]);
	}
	printf("\nrhs:\n");
	for (i=0; i<(3 * edgesCount * k + nodesCount + k); i++) {
		printf("%f ", *rhs[i]);
	}

	*/

	return TRUE;
}

bool lp_matrix(int k, int *matbeg, int *matcnt, int *matind, double *matval) {
	extern node* nodes;
	extern edge* edges;
	extern int nodesCount;
	extern int edgesCount;

	/* helper variables */
	int matbeg_offset = edgesCount * k * 3;
	int matcnt_sum = 0;
	int* scanned; /* remembering scanned nodes per edge */

	int i, j;

	scanned = calloc(sizeof(int), nodesCount);
	if (scanned == NULL) { /* calloc() failed */
		return FALSE;
	}

	/* populating matbeg, matcnt */
	for (j = 0, i = edgesCount * k; i < k * (edgesCount + nodesCount); i++, j++) {
		matbeg[i] = matbeg_offset + matcnt_sum;
		matcnt[i] = 2 * (nodes[j / k].degree) + 2;

		matcnt_sum += matcnt[i];
	}

	/* constraints 1, 2 */
	for (i = 0; i < edgesCount * k; i++) {
		int edgeId = i / k;
		matbeg[i] = i * 3;
		matcnt[i] = 3;
		matind[3 * i] = 3 * i;
		matind[3 * i + 1] = 3 * i + 1;
		matind[3 * i + 2] = 3 * i + 2;
		matval[3 * i] = 1;
		matval[3 * i + 1] = 1;
		matval[3 * i + 2] = 1;

		matind[matbeg[edgesCount * k + edges[edgeId].nodeTo]
				+ scanned[edges[edgeId].nodeTo]] = 3 * i;
		matind[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ scanned[edges[edgeId].nodeFrom]] = 3 * i + 1;
		matind[matbeg[edgesCount * k + edges[edgeId].nodeTo]
				+ scanned[edges[edgeId].nodeTo] + 1] = 3 * i + 2;
		matind[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ scanned[edges[edgeId].nodeFrom] + 1] = 3 * i + 2;

		matval[matbeg[edgesCount * k + edges[edgeId].nodeTo]
				+ scanned[edges[edgeId].nodeTo]] = -1;
		matval[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ scanned[edges[edgeId].nodeFrom]] = -1;
		matval[matbeg[edgesCount * k + edges[edgeId].nodeTo]
				+ scanned[edges[edgeId].nodeTo] + 1] = -1;
		matval[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ scanned[edges[edgeId].nodeFrom] + 1] = -1;

		scanned[edges[edgeId].nodeTo] += 2;
		scanned[edges[edgeId].nodeFrom] += 2;

	}

	/* constraints 3, 4 */
	for (i = 0; i < nodesCount * k; i++) {
		matind[matbeg[edgesCount * k + i] + scanned[i / k]]
				= edgesCount * k * 3 + i / k;
		matind[matbeg[edgesCount * k + i] + scanned[i / k] + 1]
				= edgesCount * k * 3 + nodesCount + i / nodesCount;
		matval[matbeg[edgesCount * k + i] + scanned[i / k]] = 1;
		matval[matbeg[edgesCount * k + i] + scanned[i / k] + 1] = 1;
	}

	return TRUE; /* absolutely */
}
