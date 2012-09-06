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
#define EDGE_DELIMETER '-'

/* input functions */
bool read_data(char* inputFolder);
bool read_nodes(char* nodesPath);
bool read_edges(char* edgesPath);

/* output functions */
bool init_output_folder(char* outputFolder);
bool append_clustering_result(char* resultsPath, int k, double score);
bool write_upper_bound_results(char* outputFolder, int upperBound, double weightIn, double weightOut, double* scores, int* diameters);

/* helper functions */
bool concat_path(char* dir, char* name, char** path);
bool file_lines_count(FILE * fp, int* count);
bool split_names(char* names, char* name1, char* name2);
void quicksort_cluster_scores(double* scores, int* diameters, int N);
void quicksort_cluster_sizes(int* sizes, int* ids, int N);

#endif
