/*
 * xgmml_output.c
 *
 *  Created on: Sep 2, 2012
 *      Author: amirmoua
 */

#include <stdlib.h>
#include <string.h>
#include <libxml/tree.h>
#include "xgmml_output.h"
#include "consts.h"
#include "node.h"

bool create_xgmml_stub(xmlDocPtr* pDoc) {
	extern node* nodes;
	extern int nodesCount;
	extern edge* edges;
	extern int edgesCount;

	bool success = TRUE;

	xmlNodePtr root;
	int i;
	char str[33]; /*TODO magic number */

	/*  initiate xml document */
	*pDoc = xmlNewDoc(BAD_CAST "1.0");
	if (*pDoc == NULL) {
		success = FALSE;
		goto TERMINATE;
	}

	/* craete XML root node */
	root = xmlNewNode(NULL, BAD_CAST "graph");
	xmlDocSetRootElement(*pDoc, root);

	/* set root node's properties */
	xmlNewProp(root, BAD_CAST "label", BAD_CAST "k_clustering_solution");
	xmlNewProp(root, BAD_CAST "xmlns:cy", BAD_CAST "http://www.cytoscape.org");
	xmlNewProp(root, BAD_CAST "xmlns:dc",
			BAD_CAST "http://purl.org/dc/elements/1.1/");
	xmlNewProp(root, BAD_CAST "xmlns:rdf",
			BAD_CAST "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
	xmlNewProp(root, BAD_CAST "xmlns:xlink",
			BAD_CAST "http://www.w3.org/1999/xlink");
	xmlNewProp(root, BAD_CAST "xmlns", BAD_CAST "http://www.cs.rpi.edu/XGMML");
	xmlNewProp(root, BAD_CAST "directed", BAD_CAST "0");

	/* create nodes list */
	for (i = 0; i < nodesCount; i++) {
		xmlNodePtr node;

		/* create node */
		node = xmlNewChild(root, NULL, BAD_CAST "node", NULL);
		xmlNewProp(node, BAD_CAST "label", BAD_CAST nodes[i].name);
		sprintf(str, "%d", i + 1);
		xmlNewProp(node, BAD_CAST "id", BAD_CAST str);

		/* create graphic child node */
		node = xmlNewChild(node, NULL, BAD_CAST "graphics", NULL);
		xmlNewProp(node, BAD_CAST "type", BAD_CAST "ELLIPSE");
		xmlNewProp(node, BAD_CAST "fill", BAD_CAST "#color");
		xmlNewProp(node, BAD_CAST "width", BAD_CAST "5");
		xmlNewProp(node, BAD_CAST "cy:nodeLabel", BAD_CAST nodes[i].name);
		xmlNewProp(node, BAD_CAST "cy:borderLineType", BAD_CAST "solid");
	}

	/* create edges list */
	for (i = 0; i < edgesCount; i++) {
		char* label = NULL;
		xmlNodePtr node;

		node = xmlNewChild(root, NULL, BAD_CAST "edge", NULL);

		success = edge_label(edges[i].nodeFrom, edges[i].nodeTo, &label);
		if (!success) {
			free(label);
			goto TERMINATE;
		}

		xmlNewProp(node, BAD_CAST "label", BAD_CAST label);
		free(label);

		sprintf(str, "%d", edges[i].nodeFrom + 1);
		xmlNewProp(node, BAD_CAST "source", BAD_CAST str);

		sprintf(str, "%d", edges[i].nodeTo + 1);
		xmlNewProp(node, BAD_CAST "target", BAD_CAST str);

		/* create child graphic node */
		node = xmlNewChild(node, NULL, BAD_CAST "graphics", NULL);
		sprintf(str, "weight = %1.3f", edges[i].weight);
		xmlNewProp(node, BAD_CAST "cy:edgeLabel", BAD_CAST str);

	}

TERMINATE:

/* free stuff */

	return success;
}

bool edge_label(int nodeFrom, int nodeTo, char** result) {
	extern node* nodes;

	bool success = TRUE;
	char* nameFrom = nodes[nodeFrom].name;
	char* nameTo = nodes[nodeTo].name;
	char* sep = " (interaction detected) ";

	*result = calloc(sizeof(char), strlen(nameFrom) + strlen(nameTo) + strlen(sep) + 1);
	success = (result != NULL);

	success = success && (sprintf(*result, "%s%s%s", nameFrom, sep, nameTo) > 0);

	return success;
}
