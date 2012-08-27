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

	int k;
	double score;
	bool success = TRUE;

	/* parse arguments */
	if (argc != 5) {
		perror("Usage: kcluster <inputFolder> <outputFolder> <lowerBound> <upperBound>\n");
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

	success = read_data(inputFolder);

	for (k = lowerBound; success && k <= upperBound; k++) {

		printf("trying with k=%d...\n", k);
		if ( k_cluster(k, &score) ) { /* k_cluster() failed */
			success = FALSE;
		}

		printf("\nRESSSSULT %3.3f\n====================\n", score);

		/* TODO free stuff */
	}

	return 0;
}
