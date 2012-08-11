/*
 * files.c
 *
 *  Created on: Aug 11, 2012
 *      Author: sagiemao
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "consts.h"
#include "node.h"
#include "files.h"

/* Methods */
bool read_data(char* inputFolder) {
	bool success;
	success = read_nodes(strcat(inputFolder, "nodes"));
	success = read_edges(strcat(inputFolder, "edges")) && success;
	return success;
}
bool read_nodes(char* nodesPath) {
	extern node* nodes;

	FILE * fp;
	char ch;
	int count = 0;
	int i = 0;
	char name [MAX_LINE_LENGTH];

	/* open file */
	fp = fopen(nodesPath, "r");
	if (fp == NULL) {
		perror("Error: Input file not readable.\n");
		return FALSE;
	}

	/* figure out number of nodes in file */
	ch = getc(fp);
	while (ch != EOF) {
		if (ch == '\n') {
			count++;
		}
		ch = getc(fp);
	}

	/* Initialize nodes array */
	nodes = (node*) malloc(count * sizeof(node));

	/* Loop through lines, adding nodes */
	rewind(fp);
	while (fscanf(fp, "protein: %s\n", name) == 1) {
		add_node(i++, name);
	}

	print_nodes(nodes, count);

	return TRUE;
}
bool read_edges(char* edgesPath) {
	extern edge* edges;
	return 0;
}
