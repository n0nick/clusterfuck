/*
 * files.h
 *
 *  Created on: Aug 11, 2012
 *      Author: amirmoua
 */

#ifndef filesFUCK
#define filesFUCK

#define MAX_LINE_LENGTH 500
#define NODE_PREFIX protein
#define EDGE_PREFIX interaction

bool read_data(char* inputFolder);
bool read_nodes(char* nodesPath);
bool read_edges(char* edgesPath);

#endif