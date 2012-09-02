/*
 * files.c
 *
 *  Created on: Aug 11, 2012
 *      Author: sagiemao
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/tree.h>
#include "consts.h"
#include "node.h"
#include "files.h"
#include "diameter.h"
#include "xgmml_output.h"

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

bool init_output_folder(char* outputFolder) {
	bool success = TRUE;
	FILE * fp;
	char* resultsPath;

	concat_path(outputFolder, "results", &resultsPath);

	/* open the 'results' file with the 'w' flag so that its content is reset */
	fp = fopen(resultsPath, "w");
	if (fp == NULL) {
		success = FALSE;
		goto TERMINATE;
	}

TERMINATE:
	fclose(fp);
	free(resultsPath);
	return success;
}

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

bool write_upper_bound_results(char* outputFolder, int upperBound, double weightIn, double weightOut, double* scores, int* diameters) {

	extern node* nodes;
	extern int nodesCount;
	extern edge* edges;
	extern int edgesCount;

	bool success = TRUE;
	char* resultsPath;
	FILE * fp;
	int i;

	concat_path(outputFolder, "results", &resultsPath);

	fp = fopen(resultsPath, "a");
	if (fp == NULL) {
		success = FALSE;
		goto TERMINATE;
	}

	success = (fprintf(fp, "\nThe clustered network for k=%d:\n", upperBound) > 0);

	/* nodes list */
	success = success && (fprintf(fp, "%d vertices:\n", nodesCount) > 0);
	for (i=0; (i<nodesCount) && success; i++) {
		success = success && (fprintf(fp, "%d: %s %d\n", i+1, nodes[i].name, nodes[i].clusterID+1));
	}

	/* edges list */
	success = success && (fprintf(fp, "%d edges:\n", edgesCount));
	for (i=0; (i<edgesCount) && success; i++) {
		success = success && (fprintf(fp, "%d: %s-%s %1.3f\n", i+1, nodes[edges[i].nodeFrom].name, nodes[edges[i].nodeTo].name, edges[i].weight));
	}

	/* statistics output */
	/*TODO sort clusters by score */
	success = (fprintf(fp, "\nClustering statistics for %d:\n", upperBound) > 0);
	success = success && (fprintf(fp, "Average weight of an edge within clusters: %1.3f\n", weightIn) > 0);
	success = success && (fprintf(fp, "Average weight of an edge between clusters: %1.3f\n", weightOut) > 0);

	quicksort_cluster_scores(scores, diameters, upperBound);

	for (i=0; (i<upperBound) && success; i++) {
		fprintf(fp, "Cluster %d: score - %1.3f diameter - ", i+1, scores[i]);
		if (diameters[i] == INF_DIAMETER) {
			fprintf(fp, "inf\n");
		} else {
			fprintf(fp, "%d\n", diameters[i]);
		}
	}



	if (!success) {
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

void quicksort_cluster_scores(double* scores, int* diameters, int N) {
	int i, j;
	double v, tempScore;
	int tempDiameter;

	if (N <= 1) {
		return;
	}

	v = scores[0];
	i = 0;
	j = N;
	for (;;) {
		while (scores[++i] > v && i < N) {}
		while (scores[--j] < v) {}
		if (i >= j) {
			break;
		}

		tempScore = scores[i];
		scores[i] = scores[j];
		scores[j] = tempScore;

		tempDiameter = diameters[i];
		diameters[i] = diameters[j];
		diameters[j] = tempDiameter;
	}

	tempScore = scores[i - 1];
	scores[i - 1] = scores[0];
	scores[0] = tempScore;

	tempDiameter = diameters[i - 1];
	diameters[i - 1] = diameters[0];
	diameters[0] = tempDiameter;

	quicksort_cluster_scores(scores, diameters, i - 1);
	quicksort_cluster_scores(scores + i, diameters + i, N - i);
}

void quicksort_cluster_sizes(double* sizes, int* ids, int N) {
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
