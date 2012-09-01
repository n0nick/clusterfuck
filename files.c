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

/* input methods */

bool read_data(char* inputFolder) {
	bool success;
	char* nodesPath = NULL;
	char* edgesPath = NULL;

	success = (concat_path(inputFolder, "nodes", &nodesPath) && concat_path(inputFolder, "edges", &edgesPath));

	if (success) {
		success = success && read_nodes(nodesPath);
		success = success && read_edges(edgesPath);
	}

	free(nodesPath);
	free(edgesPath);

	return success;
}
bool read_nodes(char* nodesPath) {
	extern node* nodes;
	extern int nodesCount;

	FILE * fp;
	int i = 0;
	char name[MAX_LINE_LENGTH];

	/* open file */
	fp = fopen(nodesPath, "r");
	if (fp == NULL) {
		perror("Error: Input file not readable.\n");
		return FALSE;
	}

	/* figure out number of nodes in file */
	file_lines_count(fp, &nodesCount);

	/* Initialize nodes array */
	nodes = calloc(sizeof(node), nodesCount);

	/* Loop through lines, adding nodes */
	while (fscanf(fp, "protein: %s\n", name) == 1) {
		add_node(i++, name);
	}

	return TRUE;
}
bool read_edges(char* edgesPath) {
	extern edge* edges;
	extern int edgesCount;

	char name1[MAX_LINE_LENGTH];
	char name2[MAX_LINE_LENGTH];
	char names[MAX_LINE_LENGTH];
	int idx1;
	int idx2;
	float weight;
	bool success = TRUE;
	FILE * fp;

	/* open file */
	fp = fopen(edgesPath, "r");
	if (fp == NULL) {
		perror("Error: Input file not readable.\n");
		return FALSE;
	}

	/* figure out number of edges in file */
	file_lines_count(fp, &edgesCount);

	/* allocate memory for edges array */
	edges = calloc(sizeof(edge), edgesCount);

	/* Loop through lines, adding nodes */
	while (success && (fscanf(fp, "interaction: %s %f\n", names, &weight) == 2)) {
		success = split_names(names, name1, name2);
		if (success && lookup_node(name1, &idx1) && lookup_node(name2, &idx2)) {
			success = add_edge(idx1, idx2, weight);
		}
	}

	return success;
}

/* output methods */


bool append_clustering_result(char* outputFolder, int k, double score) {
	bool success = TRUE;
	char* resultsPath;
	FILE * fp;

	concat_path(outputFolder, "results", &resultsPath);

	fp = fopen(resultsPath, "a");
	if (fp == NULL) {
		success = FALSE;
		goto TERMINATE;
	}

	if (fprintf(fp, "Clustering with k=%d: %1.3f\n", k, score) < 0) {
		success = FALSE;
		goto TERMINATE;
	}

TERMINATE:
	fclose(fp);
	free(resultsPath);
	return success;
}

/* helper methods */

bool concat_path(char* dir, char* name, char** path) {
	(*path) = (char*) malloc(strlen(dir) + strlen(name) + 1);
	if ((*path) == NULL) {
		return FALSE;
	}

	strcpy(*path, dir);
	strcat(*path, name);

	return TRUE;
}

bool file_lines_count(FILE * fp, int* count) {
	/* TODO rescue file failure */
	char ch;

	rewind(fp);
	ch = getc(fp);
	while (ch != EOF) {
		if (ch == '\n') {
			(*count)++;
		}
		ch = getc(fp);
	}
	rewind(fp);

	return TRUE;
}

bool split_names(char* names, char* name1, char* name2) {
	int i;
	int delimPos = -1;

	for (i = 0; i < strlen(names); i++) {
		if (names[i] == EDGE_DELIMETER) {
			delimPos = i;
		} else {
			if (delimPos == -1) {
				name1[i] = names[i];
			} else {
				name2[i - delimPos - 1] = names[i];
			}
		}
	}

	name1[delimPos] = '\0';
	name2[i - delimPos - 1] = '\0';

	return (strlen(name1) && strlen(name2));
}
