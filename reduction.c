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
	extern edge* edges;
	extern int edgesCount;
	int index;
	int i, cluster;

	index = 0;

	/* Z's coefficients are the edges' weights */
	for (cluster = 0; cluster < k; cluster++) {
		for (i = 0; i < edgesCount; i++) {
			(*coefficients)[cluster * edgesCount + i] = edges[i].weight;
		}
	}

	/* X's coefficients are all 0s. */

	return TRUE;
}

bool lp_rhs_sense(int k, double** rhs, char** sense) {
	extern int nodesCount;
	extern int edgesCount;

	int i;
	int numrows = (3 * edgesCount * k + nodesCount + k);

	/* constraints 1, 2 */
	for (i = 0; i < (edgesCount * k); i++) {
		(*sense)[3*i] = 'L';
		(*sense)[3*i+1] = 'L';
		(*sense)[3*i+2] = 'G';
		(*rhs)[3*i] = 0;
		(*rhs)[3*i+1] = 0;
		(*rhs)[3*i+2] = -1;
	}

	/* constraint 3 */
	for (i = 3 * edgesCount * k; i < (3 * edgesCount * k + nodesCount); i++) {
		(*rhs)[i] = 1;
		(*sense)[i] = 'E';
	}

	/* contraint 4 */
	for (; i < numrows; i++) {
		(*rhs)[i] = 1;
		(*sense)[i] = 'G';
	}

	return TRUE;
}

bool lp_matrix(int k, int **matbeg, int **matcnt, int **matind, double **matval) {
	extern node* nodes;
	extern edge* edges;
	extern int nodesCount;
	extern int edgesCount;

	/* helper variables */
	int matbeg_offset = edgesCount * k * 3;
	int matcnt_sum = 0;
	int* scanned; /* remembering scanned nodes per edge */

	int i, j;

    /* allocate memory: scanned */
	scanned = calloc(sizeof(int), nodesCount * k);
	if (scanned == NULL) {
		return FALSE;
	}

	/* populating matbeg, matcnt */
	for (j = 0, i = edgesCount * k; i < k * (edgesCount + nodesCount); i++, j++) {
		(*matbeg)[i] = matbeg_offset + matcnt_sum;
		(*matcnt)[i] = 2 * (nodes[j % nodesCount].degree) + 2;

		matcnt_sum += (*matcnt)[i];
	}

	/* constraints 1, 2 */
	for (i = 0; i < edgesCount * k; i++) {
		int edgeId = i % edgesCount;
		int clusterId = i / edgesCount;
		int edgeOffset1, edgeOffset2;
		int beg1, beg2;

		(*matbeg)[i] = i * 3;
		(*matcnt)[i] = 3;
		(*matind)[3 * i] = 3 * i;
		(*matind)[3 * i + 1] = 3 * i + 1;
		(*matind)[3 * i + 2] = 3 * i + 2;
		(*matval)[3 * i] = 1;
		(*matval)[3 * i + 1] = 1;
		(*matval)[3 * i + 2] = 1;

		/* vertex offsets in the variables of type X */
		edgeOffset1 = edges[edgeId].nodeTo + nodesCount * clusterId;
		edgeOffset2 = edges[edgeId].nodeFrom + nodesCount*clusterId;

		beg1 = (*matbeg)[edgesCount * k + edgeOffset1];
		beg2 = (*matbeg)[edgesCount * k + edgeOffset2];

		(*matind)[beg1 + scanned[edgeOffset1]] = 3*i;
		(*matind)[beg2 + scanned[edgeOffset2]] = 3*i+1;
		(*matind)[beg1 + scanned[edgeOffset1]+1] = 3*i+2;
		(*matind)[beg2 + scanned[edgeOffset2]+1] = 3*i+2;

		(*matval)[beg1 + scanned[edgeOffset1]] = -1;
		(*matval)[beg2 + scanned[edgeOffset2]] = -1;
		(*matval)[beg1 + scanned[edgeOffset1]+1] = -1;
		(*matval)[beg2 + scanned[edgeOffset2]+1] = -1;

		scanned[edgeOffset1] += 2;
		scanned[edgeOffset2] += 2;

	}

	/* constraints 3, 4 */
	for (i = 0; i < nodesCount * k; i++) {
		(*matind)[(*matbeg)[edgesCount * k + i] + scanned[i]] = edgesCount * k * 3
				+ i % nodesCount;
		(*matind)[(*matbeg)[edgesCount * k + i] + scanned[i] + 1] = edgesCount * k
				* 3 + nodesCount + i / nodesCount;
		(*matval)[(*matbeg)[edgesCount * k + i] + scanned[i]] = 1;
		(*matval)[(*matbeg)[edgesCount * k + i] + scanned[i] + 1] = 1;
	}

	return TRUE; /* absolutely */
}

bool lp_bounds(int numcols, double **lb, double **ub) {
	int i;

	/* populate lb (all 0's) and ub (all 1's) */
	for (i = 0; i < numcols; i++) {
		(*lb)[i] = 0;
		(*ub)[i] = 1;
	}

	return TRUE;
}

bool lp_indices_types(int numcols, int **indices, char **types, char type) {
	int i;

	for (i = 0; i < numcols; i++) {
		(*indices)[i] = i;
		(*types)[i] = type;
	}

	return TRUE;
}
