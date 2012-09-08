/*
 * xgmml_output.h
 *
 *  Created on: Sep 2, 2012
 *      Author: amirmoua
 */

#ifndef xgmmlFUCK
#define xgmmlFUCK

#include "consts.h"

#define BEST_CLUSTER_THRESHOLD 5

bool create_xgmml_stub(xmlDocPtr* pDoc);
bool edge_label(int nodeFrom, int nodeTo, char** result);
bool create_cluster_xgmml(int k, xmlDocPtr stub, char* outputFolder, int* clustersOrdered, bool best);

#endif /* xgmmlFUCK */
