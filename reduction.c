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

bool lp_objective_function_coefficients(int k, double* coefficients) {
	extern node* nodes;
	extern int nodesCount;
	int i, j;
	edge* currEdge;

	/* X's coefficients are all 0s. */
	for (i = 0; i < (k * nodesCount); i++) {
		coefficients[i] = 0;
	}

	/* Z's coefficients are the edges' weights */
	for (j = 0; j < nodesCount; j++) {
		currEdge = nodes[j].edges;
		while (currEdge != NULL) {
			coefficients[i++] = currEdge->weight;
			currEdge = currEdge->next;
		}
	}

	return TRUE;
}

bool lp_params(int k, double **rhs_p, double **matval_p,
		double **lb_p, double **ub_p, int **matbeg_p, int **matcnt_p,
		int **matind_p, char ** sense_p, int *numCols_p, int *numRows_p,
		int ** indices_p, char ** ctypes_p) {
	int numcols, numrows, three_k_e, i, j, x, constraints_cnt;
	int * arr_deg, *track_arr, *matbeg, *matind, *matcnt, *indices;
	double *rhs, *matval, *lb, *ub;
	char * sense, *ctypes;

	extern node* nodes;
	extern int edgesCount;
	extern int nodesCount;

	edge* currEdge;
	edge** edges = (edge**) malloc(edgesCount * sizeof(edge*));
	j = 0;

	for (i = 0; i < nodesCount; i++) {
		currEdge = nodes[i].edges;
		while (currEdge != NULL) {
			edges[j++] = currEdge;
			currEdge = currEdge->next;
		}
	}

	numcols = k * (edgesCount + nodesCount); /*NUM_OF_VARS*/
	numrows = 3 * k * edgesCount + nodesCount + k; /*nUM_OF_CONSTRAINTS*/
	lb = (double*) malloc(numcols * sizeof(double));
	ub = (double*) malloc(numcols * sizeof(double));
	sense = (char*) malloc(sizeof(char) * numrows);
	rhs = (double*) malloc(sizeof(double) * numrows);
	matbeg = (int*) malloc(sizeof(int) * numcols);
	matcnt = (int*) malloc(sizeof(int) * numcols);
	matval = (double*) malloc(sizeof(double) * (7 * k * edgesCount + 2 * k
			* nodesCount));
	matind = (int*) malloc(sizeof(int) * (7 * k * edgesCount + 2 * k
			* nodesCount));
	three_k_e = 3 * edgesCount * k;
	arr_deg = (int*) calloc(nodesCount, sizeof(int));
	ctypes = (char*) malloc(numcols * sizeof(char));
	indices = (int *) malloc(numcols * sizeof(int));
	/*TODO: Asserts!!!*/

	/*Now building an array, holding for place i, the degree of vertex i */

	for (i = 0; i < edgesCount; i++) {
		arr_deg[edges[i]->nodeFrom]++;
		arr_deg[edges[i]->nodeTo]++;
	}

	/*assigning the first K*e mat_cnt*/
	for (i = 0; i < edgesCount; i++) {
		for (j = 0; j < k; j++) {
			matcnt[k * i + j] = 3;
		}
	}

	/*assigning the rest v* k mat_cnt*/
	for (i = 0; i < nodesCount; i++) {
		for (j = 0; j < k; j++) {
			matcnt[(k * edgesCount) + k * i + j] = arr_deg[i] * 2 + 2;
		}
	}

	/*Now assigning the matbeg by the mat_cnt array*/
	matbeg[0] = 0;
	for (i = 1; i < numcols; i++) {
		matbeg[i] = matcnt[i - 1] + matbeg[i - 1];
	}

	/* assigning matind & matval */

	for (i = 0; i < three_k_e; i++) {
		matval[i] = 1;
	}

	constraints_cnt = 0;
	for (i = 0; i < nodesCount; i++) {
		for (j = 0; j < k; j++) {
			for (x = 0; x < arr_deg[i] * 2; x++) {
				matval[three_k_e + constraints_cnt] = -1;
				constraints_cnt++;
			}

			matval[three_k_e + constraints_cnt] = 1;
			constraints_cnt++;
			matval[three_k_e + constraints_cnt] = 1;
			constraints_cnt++;
		}
	}

	constraints_cnt = 0;
	track_arr = (int *) calloc(sizeof(int), numcols);
	for (i = 0; i < edgesCount; i++) {
		for (j = 0; j < k; j++) {
			int iv1, iv2;
			matind[3 * (i * k + j)] = 2 * (i * k + j);
			matind[3 * (i * k + j) + 1] = 2 * (i * k + j) + 1;
			matind[3 * (i * k + j) + 2] = 2 * k * edgesCount + (i * k + j);

			iv1 = k * edgesCount + edges[i]->nodeFrom * k + j;
			iv2 = k * edgesCount + edges[i]->nodeTo * k + j;

			matind[matbeg[iv1] + track_arr[iv1]] = 2 * (i * k + j);
			track_arr[iv1]++;

			matind[matbeg[iv2] + track_arr[iv2]] = 2 * (i * k + j) + 1;
			track_arr[iv2]++;

		}
	}

	for (i = 0; i < edgesCount; i++) {
		for (j = 0; j < k; j++) {
			int iv1, iv2;

			iv1 = k * edgesCount + edges[i]->nodeFrom * k + j;
			iv2 = k * edgesCount + edges[i]->nodeTo * k + j;
			matind[matbeg[iv1] + track_arr[iv1]] = 2 * k * edgesCount + (i * k
					+ j);
			track_arr[iv1]++;
			matind[matbeg[iv2] + track_arr[iv2]] = 2 * k * edgesCount + (i * k
					+ j);
			track_arr[iv2]++;
		}
	}

	for (i = 0; i < nodesCount; i++) {
		for (j = 0; j < k; j++) {
			matind[matbeg[k * edgesCount + i * k + j] + matcnt[k * edgesCount
					+ i * k + j] - 2] = three_k_e + i;
			matind[matbeg[k * edgesCount + i * k + j] + matcnt[k * edgesCount
					+ i * k + j] - 1] = three_k_e + nodesCount + j;
		}
	}

	for (i = 0; i < 2 * k * edgesCount; ++i) {
		rhs[i] = 0;
		sense[i] = 'L';
	}
	for (i = i; i < 3 * k * edgesCount; ++i) {
		rhs[i] = -1;
		sense[i] = 'G';
	}

	for (i = three_k_e; i < three_k_e + nodesCount; i++) {
		rhs[i] = 1;
		sense[i] = 'E';
	}
	for (i = three_k_e + nodesCount; i < three_k_e + nodesCount + k; i++) {
		rhs[i] = 1;
		sense[i] = 'G';
	}

	for (i = 0; i < numcols; i++) {
		lb[i] = 0;
		ub[i] = 1;
		indices[i] = i;
		ctypes[i] = 'B';
	}

	/*TODO:More frees!!@!FF*/

	*matind_p = matind;
	*matval_p = matval;
	*lb_p = lb;
	*ub_p = ub;
	*matbeg_p = matbeg;
	*matcnt_p = matcnt;
	*sense_p = sense;
	*numCols_p = numcols;
	*numRows_p = numrows;
	*rhs_p = rhs;
	*ctypes_p = ctypes;
	*indices_p = indices;

	return TRUE;
}
