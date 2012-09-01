/*
 * diameter.c
 *
 *  Created on: Sep 1, 2012
 *      Author: sagiemao
 */

#include <stdlib.h>
#include "diameter.h"
#include "queue.h"
#include "node.h"

int node_cluster_diameter(int nodeID, int* visited) {
	extern node* nodes;
	extern int nodesCount;

	queue q;
	int i, u, v, max;
	edge* currEdge;

	init_queue(&q);

	for (i = 0; i < nodesCount; i++) {
		visited[i] = -1;
	}

	visited[nodeID] = 0;
	push(&q, nodeID);

	while (q.count != 0) {
		u = pop(&q);

		currEdge = nodes[u].edges;
		while (currEdge != NULL) {
			v = currEdge->nodeTo;
			if (nodes[v].clusterID == nodes[nodeID].clusterID &&
					visited[v] == -1) {
				visited[v] = visited[u] + 1;
				push(&q, v);
			}

			currEdge = currEdge->next;
		}
	}

	/* find max. diameter */
	max = visited[0];
	for (i = 1; i < nodesCount; i++) {
		if (visited[i] > max) {
			max = visited[i];
		}
	}

	return max;
}

int cluster_diameter(int clusterID) {
	extern node* nodes;
	extern int nodesCount;

	int i, max, diam;
	int* visited = calloc(sizeof(int), nodesCount);

	max = node_cluster_diameter(0, visited);
	for (i = 1; i < nodesCount; i++) {
		if (nodes[i].clusterID == clusterID) {
			diam = node_cluster_diameter(i, visited);
			if (diam > max) {
				max = diam;
			}
		}
	}
	return max;
}
