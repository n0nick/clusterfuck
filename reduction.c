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

	int i;
	int size = (3 * edgesCount * k + nodesCount + k);

	rhs = calloc(sizeof(double*), size);
	sense = calloc(sizeof(char*), size);
	for (i = 0; i < size; i++) {
		rhs[i] = calloc(sizeof(double), 1);
		sense[i] = calloc(sizeof(char), 1);
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

	int place_taken_in_matval = edgesCount * k * 3;
	int temp_sum = 0;

	int* vertices_so_far = calloc(sizeof(int), nodesCount);

	int i, j;

	for (j = 0, i = edgesCount * k; i < edgesCount * k + k * nodesCount; i++, j++) {
		matbeg[i] = place_taken_in_matval + temp_sum;
		matcnt[i] = 2 * (nodes[j / k].degree) + 2;

		temp_sum += matcnt[i];
	}

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
				+ vertices_so_far[edges[edgeId].nodeTo]] = 3 * i;
		matind[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ vertices_so_far[edges[edgeId].nodeFrom]] = 3 * i + 1;
		matind[matbeg[edgesCount * k + edges[edgeId].nodeTo]
				+ vertices_so_far[edges[edgeId].nodeTo] + 1] = 3 * i + 2;
		matind[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ vertices_so_far[edges[edgeId].nodeFrom] + 1] = 3 * i + 2;

		matval[matbeg[edgesCount * k + edges[edgeId].nodeTo]
				+ vertices_so_far[edges[edgeId].nodeTo]] = -1;
		matval[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ vertices_so_far[edges[edgeId].nodeFrom]] = -1;
		matval[matbeg[edgesCount * k + edges[edgeId].nodeTo]
				+ vertices_so_far[edges[edgeId].nodeTo] + 1] = -1;
		matval[matbeg[edgesCount * k + edges[edgeId].nodeFrom]
				+ vertices_so_far[edges[edgeId].nodeFrom] + 1] = -1;

		vertices_so_far[edges[edgeId].nodeTo] += 2;
		vertices_so_far[edges[edgeId].nodeFrom] += 2;

	}

	/*fill matind x varibles for constraints of types III and IV*/
	for (i = 0; i < nodesCount * k; i++) {
		matind[matbeg[edgesCount * k + i] + vertices_so_far[i / k]]
				= edgesCount * k * 3 + i / k;
		matind[matbeg[edgesCount * k + i] + vertices_so_far[i / k] + 1]
				= edgesCount * k * 3 + nodesCount + i / nodesCount;
		matval[matbeg[edgesCount * k + i] + vertices_so_far[i / k]] = 1;
		matval[matbeg[edgesCount * k + i] + vertices_so_far[i / k] + 1] = 1;
	}

	return TRUE; /* absolutely */
}
