/*
 * main.c
 *
 *  Created on: Aug 11, 2012
 *      Author: amirmoua
 */

#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "files.h"
#include "cluster.h"
#include "diameter.h"

/* Global variables */
node* nodes = NULL;
edge* edges = NULL;
int nodesCount = 0;
int edgesCount = 0;

/* main */
int main(int argc, char* argv[]) {
	char *inputFolder;
	char *outputFolder;
	int lowerBound;
	int upperBound;

	bool success = TRUE;
	int k;
	double score;

	double weightIn, weightOut;
	double* scores;
	int* diameters;
	int i;

	/* parse arguments */
	if (argc != 5) {
		perror(
				"Usage: kcluster <inputFolder> <outputFolder> <lowerBound> <upperBound>\n");
		return 1;
	}
	inputFolder = argv[1];
	outputFolder = argv[2];
	lowerBound = atoi(argv[3]);
	upperBound = atoi(argv[4]);

	/* validate arguments */
	if (lowerBound == 0 || upperBound == 0) {
		perror("Error: cluster size bounds should be positive integers.\n");
		return 1;
	}
	if (lowerBound > upperBound) {
		perror("Error: lower bound should be lower than upper bound.\n");
		return 1;
	}

	/* reading input and initializing output folder */
	success = read_data(inputFolder);
	success = success && init_output_folder(outputFolder);

	/* for each cluster, find a solution and save result */
	for (k = lowerBound; success && k <= upperBound; k++) {

		if ( k_cluster(k, &score) ) { /* k_cluster() failed */
			success = FALSE;
			goto TERMINATE;
		}

		success = append_clustering_result(outputFolder, k, score);

		success = success && write_xgmml_file(k);

		/* TODO free stuff */
	}

	if (!success) {
		goto TERMINATE;
	}

	/* calculate statistic values */
	scores = calloc(sizeof(double), upperBound);
	success = clustering_statistics(&weightIn, &weightOut, scores);

	diameters = calloc(sizeof(int), upperBound);
	for(i = 0 ; i<upperBound ;i++) {
		diameters[i] = cluster_diameter(i);
	}

	/* finish the 'results' file */
	success = success && write_upper_bound_results(outputFolder, upperBound, weightIn, weightOut, scores, diameters);

TERMINATE:

	/* TODO free each node's name and edges list */
	free(nodes);
	free(edges);
	free(scores);

	return 0;
}
