/*
 * diameter.h
 *
 *  Created on: Sep 1, 2012
 *      Author: sagiemao
 */

#ifndef diameterEX3
#define diameterEX3

#include "consts.h"

#define INF_DIAMETER -1

bool node_cluster_diameter(int nodeID, int* visited, int* max);
bool cluster_diameter(int clusterID, int* max);

#endif /* diameterEX3 */
