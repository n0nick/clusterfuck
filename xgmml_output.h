/*
 * xgmml_output.h
 *
 *  Created on: Sep 2, 2012
 *      Author: amirmoua
 */

#ifndef xgmmlEX3
#define xgmmlEX3

#include "consts.h"

#define BEST_CLUSTER_THRESHOLD 5
#define MAX_NODE_ID_LENGTH 10

bool create_xgmml_stub(xmlDocPtr* pDoc);
bool edge_label(int nodeFrom, int nodeTo, char** result);
bool create_cluster_xgmml(int k, xmlDocPtr stub, char* outputFolder, int* clustersOrdered, bool best);

#endif /* xgmmlEX3 */
