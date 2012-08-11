#ifndef CLUSTER_H
#define CLUSTER_H

/* You may and should alter the file */

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with include of cplex.h. */
#include <ilcplex/cplex.h>
#include <stdlib.h>

/* Bring in the declarations for the string functions */
#include <string.h>

/* Include declaration for functions */
void free_and_null (char **ptr);
int  k_cluster();

#endif