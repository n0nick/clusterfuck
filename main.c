/*
 * main.c
 *
 *  Created on: Aug 11, 2012
 *      Author: amirmoua
 */

#include <stdio.h>
#include <stdlib.h>
#include <libxml/tree.h>
#include "node.h"
#include "files.h"
#include "cluster.h"
#include "diameter.h"
#include "xgmml_output.h"

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
	int *clustersOrdered, *clusterIds;
	int i;

	edge* currEdge;
	edge* tmpEdge;

	xmlDocPtr stub = NULL;

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

	success = success && create_xgmml_stub(&stub);

	/* for each cluster, find a solution and save result */
	for (k = lowerBound; success && k <= upperBound; k++) {

		if ( k_cluster(k, &score, outputFolder) ) { /* k_cluster() failed */
			success = FALSE;
			goto TERMINATE;
		}

		/* order clusters by size */
		success = success && clusters_list(k, &clusterIds, &clustersOrdered);
		success = success && append_clustering_result(outputFolder, k, score);
		success = success && create_cluster_xgmml(k, stub, outputFolder, clustersOrdered, FALSE);
		if (k != upperBound) {
			free(clusterIds);
			free(clustersOrdered);
		}
	}

	if (!success) {
		goto TERMINATE;
	}

	/* calculate statistic values */
	scores = calloc(sizeof(double), upperBound);
	success = clustering_statistics(&weightIn, &weightOut, scores);

	diameters = calloc(sizeof(int), upperBound);
	for(i = 0 ; (i<upperBound) && success ;i++) {
		success = success && cluster_diameter(i, &(diameters[i]));
	}

	/* finish the 'results' file */
	success = success && write_upper_bound_results(outputFolder, upperBound, weightIn, weightOut, scores, diameters, clusterIds);

	/* create best_clusters file */
	success = success && create_cluster_xgmml(upperBound, stub, outputFolder, clustersOrdered, TRUE);

TERMINATE:

	/* free each node's name and edges list */
	for(i=0; i<nodesCount; i++) {
		free(nodes[i].name);
		currEdge = nodes[i].edges;
		while(currEdge != NULL) {
			tmpEdge = currEdge->next;
			free(currEdge);
			currEdge = tmpEdge;
		}
	}
	/* free nodes & edges arrays */
	free(nodes);
	free(edges);
	/* free clustering stats arrays */
	free(scores);
	free(diameters);
	free(clusterIds);
	free(clustersOrdered);
	/* free xml document pointers */
	xmlFreeDoc(stub);
	xmlCleanupParser();
	xmlMemoryDump();

	return 0;
}
