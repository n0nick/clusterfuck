/*------------------------------------------------------------------------*/
/*  File: cluster.c                                                       */
/*  Version 1.0                                                           */
/*------------------------------------------------------------------------*/
/*  Written for Software Project Class.                                   */
/*  Based uopn qpex1.c                                                    */
/*  Permission is expressly granted to use this example in the            */
/*  course of developing applications that use ILOG products.             */
/*------------------------------------------------------------------------*/

/* cluster.c - Entering and optimizing k-clustering integer linear programming problem */
/* You may and should alter the file */

#include "cluster.h"
#include "reduction.h"

#include "node.h"

/* This routine initializes the cplex enviorement, sets screen as an output for cplex errors and notifications, 
 and sets parameters for cplex. It calls for a mixed integer program solution and frees the environment.
 To Do:
 Declare the parameters for the problem and fill them accordingly. After creating the program thus, copy it into cplex.
 Define any integer or binary variables as needed, and change their type before the call to CPXmipopt to solve problem.
 Use CPXwriteprob to output the problem in lp format, in the name of <k>.lp.
 Read solution (both objective function value and variables assignment).
 Communicate to pass the problem and the solution between the modules in the best way you see.
 */
int k_cluster(int k, double *score) {
	extern int nodesCount;
	extern int edgesCount;

	extern node* nodes;

	/* Declare pointers for the variables and arrays that will contain
	 the data which define the LP problem. */

	char *probname = NULL;

	/* Declare and allocate space for the variables and arrays where we
	 will store the optimization results including the status, objective
	 value and variable values. */

	CPXENVptr p_env = NULL;
	CPXLPptr p_lp = NULL;
	int status;
	bool success = TRUE;

	/* problem variables */
	int numcols;
	int numrows;
	double *coeffs;
	char *sense;
	double *rhs;
	int *matbeg;
	int *matcnt;
	int *matind;
	double *matval;
	double *lb;
	double *ub;
	int *indices;
	char *types;

	int solutionStatus;

	int i;

	/* prepare problem name */
	probname = calloc(sizeof(char), strlen("k_cluster.LP") + 10);
	/*TODO check calloc success */
	sprintf(probname, "k_cluster_%08d.LP", k);

	/* Initialize the CPLEX environment */
	p_env = CPXopenCPLEX(&status);

	/* If an error occurs, the status value indicates the reason for
	 failure.  A call to CPXgeterrorstring will produce the text of
	 the error message. Note that CPXopenCPLEX produces no output,
	 so the only way to see the cause of the error is to use
	 CPXgeterrorstring. For other CPLEX routines, the errors will
	 be seen if the CPX_PARAM_SCRIND indicator is set to CPX_ON.  */

	if (p_env == NULL) {
		char errmsg[1024];
		fprintf(stderr, "Error: Could not open CPLEX environment.\n");
		CPXgeterrorstring(p_env, status, errmsg);
		fprintf(stderr, "%s", errmsg);
		goto TERMINATE;
	}

	/* Turn on output to the screen */
	status = CPXsetintparam(p_env, CPX_PARAM_SCRIND, CPX_ON);
	if (status) {
		fprintf(stderr,
				"Error: Failure to turn on screen indicator, error %d.\n",
				status);
		goto TERMINATE;
	}

	/* Create the problem. */
	p_lp = CPXcreateprob(p_env, &status, probname);

	/* A returned pointer of NULL may mean that not enough memory
	 was available or there was some other problem.  In the case of
	 failure, an error message will have been written to the error
	 channel from inside CPLEX. The setting of
	 the parameter CPX_PARAM_SCRIND causes the error message to
	 appear on stdout.  */

	if (p_lp == NULL) {
		fprintf(stderr, "Error: Failed to create problem.\n");
		goto TERMINATE;
	}

	/* Prepare problem variables */
	/* TODO check success */
	numcols = k * (nodesCount + edgesCount);
	numrows = 3 * edgesCount * k + nodesCount + k;

	coeffs = calloc(sizeof(double), numcols);
	if (coeffs == NULL) {
		fprintf(stderr, "Error: Failed to allocate memory to coeffs.\n");
		goto TERMINATE;
	}
	success = success && lp_objective_function_coefficients(k, &coeffs);

	rhs = calloc(sizeof(double), numrows);
	sense = calloc(sizeof(char), numrows);
	success = success && lp_rhs_sense(k, &rhs, &sense);

	matbeg = calloc(sizeof(int), numcols);
	matcnt = calloc(sizeof(int), numcols);
	matind = calloc(sizeof(int), k * (edgesCount * 7 + nodesCount * 2));
	matval = calloc(sizeof(double), k * (edgesCount * 7 + nodesCount * 2));
	if ((matbeg == NULL) || (matcnt == NULL) || (matind == NULL) || (matval
			== NULL)) {
		fprintf(stderr,
				"Error: Failed to allocate memory to the constraints.\n");
		goto TERMINATE;
	}
	success = success && lp_matrix(k, &matbeg, &matcnt, &matind, &matval);

	lb = calloc(sizeof(double), numcols);
	ub = calloc(sizeof(double), numcols);
	if ((lb == NULL) || (ub == NULL)) {
		fprintf(stderr,
				"Error: Failed to allocate memory to lower/upper bounds.\n");
		goto TERMINATE;
	}
	success = success && lp_bounds(numcols, &lb, &ub);

	/* If success is TRUE then ALL preparation stages were executed correctly */
	if (!success) {
		fprintf(stderr, "Error: Initializing the problems' arrays failed");
		goto TERMINATE;
	}

	indices = calloc(sizeof(int), numcols);
	types = calloc(sizeof(char), numcols);
	lp_indices_types(numcols, &indices, &types, CPX_BINARY);

	/* Use CPXcopylp to transfer the ILP part of the problem data into the cplex pointer lp */
	status = CPXcopylp(p_env, p_lp, numcols, numrows, CPX_MAX, coeffs, rhs,
			sense, matbeg, matcnt, matind, matval, lb, ub, NULL);

	if (status) {
		fprintf(
				stderr,
				"Error: Failed to transfer the ILP part of the problem data into the cplex pointer lp.\n");
		goto TERMINATE;
	}

	status = CPXchgctype(p_env, p_lp, numcols, indices, types);
	if (status) {
		fprintf(stderr, "Error: Failed to change variable type.\n");
		goto TERMINATE;
	}

	/* Optimize the problem. */
	status = CPXmipopt(p_env, p_lp);
	if (status) {
		fprintf(stderr, "Error: Failed to optimize problem.\n");
		goto TERMINATE;
	}

	/* Create the solution */
	status = CPXsolution(p_env, p_lp, &solutionStatus, score, coeffs, NULL,
			NULL, NULL);
	if (status) {
		fprintf(stderr, "Error: Failed to read solution.\n");
		goto TERMINATE;
	}

	for (i = 0; i < k * nodesCount; i++) {
		if (IS_VALUE_1(coeffs[k * edgesCount + i])) {
			nodes[i % nodesCount].clusterID = i / nodesCount;
		}
	}

	/* Write a copy of the problem to a file. */
	status = CPXwriteprob(p_env, p_lp, probname, NULL);
	if (status) {
		fprintf(stderr, "Error: Failed to write LP to disk.\n");
		goto TERMINATE;
	}

TERMINATE:

	/* Free up the problem as allocated by CPXcreateprob, if necessary */
	if (p_lp != NULL) {
		status = CPXfreeprob(p_env, &p_lp);
		if (status) {
			fprintf(stderr, "Error: CPXfreeprob failed, error code %d.\n",
					status);
		}
	}

	/* Free up the CPLEX environment, if necessary */
	if (p_env != NULL) {
		status = CPXcloseCPLEX(&p_env);

		/* Note that CPXcloseCPLEX produces no output,
		 so the only way to see the cause of the error is to use
		 CPXgeterrorstring.  For other CPLEX routines, the errors will
		 be seen if the CPX_PARAM_SCRIND indicator is set to CPX_ON. */

		if (status) {
			char errmsg[1024];
			fprintf(stderr, "Could not close CPLEX environment.\n");
			CPXgeterrorstring(p_env, status, errmsg);
			fprintf(stderr, "%s", errmsg);
		}
	}

	/* Free up the problem data arrays, if necessary. */
	free(probname);
	free(coeffs);
	free(sense);
	free(rhs);
	free(matbeg);
	free(matcnt);
	free(matind);
	free(matval);
	free(lb);
	free(ub);
	free(indices);
	free(types);

	return (status);
}

bool clustering_statistics(double* insideAvg, double* outsideAvg, double* scores) {
	extern node* nodes;
	extern edge* edges;
	extern int edgesCount;

	int i;
	int countIn = 0;
	int countOut = 0;
	*insideAvg = 0;
	*outsideAvg = 0;

	for (i=0; i<edgesCount; i++) {
		if (nodes[edges[i].nodeFrom].clusterID == nodes[edges[i].nodeTo].clusterID) {
			/* inside a cluster */
			(*insideAvg) += edges[i].weight;
			countIn++;

			scores[nodes[edges[i].nodeFrom].clusterID]+= edges[i].weight;
		} else {
			/* between clusters */
			(*outsideAvg) += edges[i].weight;
			countOut++;
		}
	}

	if (countIn != 0) {
		(*insideAvg) /= countIn;
	}
	if (countOut != 0) {
		(*outsideAvg) /= countOut;
	}

	return TRUE;
}

void quicksort_cluster_sizes(int* sizes, int* ids, int N) {
	int i, j;
	int v, tempSize, tempId;

	if (N <= 1) {
		return;
	}

	v = sizes[0];
	i = 0;
	j = N;
	for (;;) {
		while (sizes[++i] > v && i < N) {}
		while (sizes[--j] < v) {}
		if (i >= j) {
			break;
		}

		tempSize = sizes[i];
		sizes[i] = sizes[j];
		sizes[j] = tempSize;

		tempId = ids[i];
		ids[i] = ids[j];
		ids[j] = tempId;
	}

	tempSize = sizes[i - 1];
	sizes[i - 1] = sizes[0];
	sizes[0] = tempSize;

	tempId = ids[i - 1];
	ids[i - 1] = ids[0];
	ids[0] = tempId;

	quicksort_cluster_sizes(sizes, ids, i - 1);
	quicksort_cluster_sizes(sizes + i, ids + i, N - i);
}

bool clusters_list(int clustersCount, int** clusterIds, int** clustersOrdered) {
	extern node* nodes;
	extern int nodesCount;

	bool success = TRUE;
	int *clusterSizes;
	int i;

	(*clusterIds) = calloc(sizeof(int), clustersCount);
	clusterSizes = calloc(sizeof(int), clustersCount);
	(*clustersOrdered) = calloc(sizeof(int), clustersCount);

	if (clusterIds == NULL || clusterSizes == NULL || clustersOrdered == NULL) {
		success = FALSE;
		goto TERMINATE;
	}

	for (i=0; i<clustersCount; i++) {
		(*clusterIds)[i] = i;
		clusterSizes[i] = 0;
	}

	for (i=0; i<nodesCount; i++) {
		clusterSizes[nodes[i].clusterID]++;
	}

	quicksort_cluster_sizes(clusterSizes, *clusterIds, clustersCount);

	for (i=0; i < clustersCount; i++) {
		(*clustersOrdered)[(*clusterIds)[i]] = i;
	}

	TERMINATE:
	free(clusterSizes);

	return success;
}
