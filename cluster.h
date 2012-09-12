#ifndef CLUSTER_H
#define CLUSTER_H

#define IS_VALUE_1(X) ((1 - X) < 0.001)

/* You may and should alter the file */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with include of cplex.h. */
#include <ilcplex/cplex.h>
#include <stdlib.h>

/* Bring in the declarations for the string functions */
#include <string.h>

#include "consts.h"

typedef struct cluster cluster;

struct cluster {
	int id;
	int size;
	double score;
};

/* Include declaration for functions */
int k_cluster(int k, double* score, char* outputFolder);
bool clustering_statistics(double* insideAvg, double* outsideAvg, double* scores);
void quicksort_cluster_sizes(int* sizes, int* ids, int N);
bool clusters_list(int clustersCount, int** clusterIds, int** clustersOrdered);
int cluster_size_comparator(const void * a, const void * b);

#endif
