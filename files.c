/*
 * files.c
 *
 *  Created on: Aug 11, 2012
 *      Author: sagiemao
 */

#include <string.h>
#include "consts.h"
#include "node.h"
#include "files.h"

/* Methods */
int read_data(char* inputFolder) {
	bool success;
	success = read_nodes(strcat(inputFolder, "nodes"));
	success = read_edges(strcat(inputFolder, "edges")) && success;
	return success;
}
int read_nodes(char* nodesPath) {
	extern node* nodes;
	return 0;
}
int read_edges(char* edgesPath) {
	extern edge* edges;
	return 0;
}
