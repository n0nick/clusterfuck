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

#include <stdio.h>

bool node_cluster_diameter(int nodeID, int* visited, int* max) {
	extern node* nodes;
	extern int nodesCount;

	queue q;
	int i, u, v;
	edge* currEdge;

	queue_init(&q);

	for (i = 0; i < nodesCount; i++) {
		visited[i] = -1;
	}
if(nodes[nodeID].clusterID == 2) {
	printf("hurrrrr");
}
	visited[nodeID] = 0;
	queue_push(&q, nodeID);

	while (q.count != 0) {
		u = queue_pop(&q);

		currEdge = nodes[u].edges;
		while (currEdge != NULL) {
			v = currEdge->nodeTo;

			/*if (v < currEdge->nodeFrom) {*/
				if ((nodes[v].clusterID == nodes[nodeID].clusterID) &&
						(visited[v] == -1)) {
					visited[v] = visited[u] + 1;
					queue_push(&q, v);
				}
			/*}*/
			currEdge = currEdge->next;
		}
	}

	/* find max. diameter */
	(*max) = visited[0];
	for (i = 1; i < nodesCount; i++) {
		if (visited[i] > (*max)) {
			(*max) = visited[i];
		}
	}

	queue_free(&q);

	return TRUE;
}

bool cluster_diameter(int clusterID, int* max) {
	extern node* nodes;
	extern int nodesCount;

	bool success = TRUE;

	int i, diam, count;
	int* visited = calloc(sizeof(int), nodesCount);

	*max = -1;

	if (visited == NULL) {
		success = FALSE;
		goto TERMINATE;
	}

	count = 0;
	for (i = 0; success && (i < nodesCount); i++) {
		if (nodes[i].clusterID == clusterID) {
			success = success && node_cluster_diameter(i, visited, &diam);
			if (diam > (*max)) {
				(*max) = diam;
			}
			count++;
		}
	}

	if ((*max) == 0 && count > 1) {
		(*max) = INF_DIAMETER;
	}

	TERMINATE:
	free(visited);

	return success;
}
