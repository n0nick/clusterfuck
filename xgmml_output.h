/*
 * xgmml_output.h
 *
 *  Created on: Sep 2, 2012
 *      Author: amirmoua
 */

#ifndef xgmmlFUCK
#define xgmmlFUCK

#include "consts.h"

bool create_xgmml_stub(xmlDocPtr* pDoc);
bool edge_label(int nodeFrom, int nodeTo, char** result);
bool create_clustering_xgmml(int k, xmlDocPtr stub, char* outputFolder);
bool clusters_list(int clustersCount, int* clusterIds, int* clusterSizes);

#endif /* xgmmlFUCK */
