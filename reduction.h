#ifndef reductionEX3
#define reductionEX3

#include "consts.h"

bool lp_objective_function_coefficients(int k, double** coefficients);
bool lp_rhs_sense(int k, double** rhs, char** sense);
bool lp_matrix(int k, int **matbeg, int **matcnt, int **matind, double **matval);
bool lp_bounds(int numcols, double **lb, double **ub);
bool lp_indices_types(int numcols, int **indices, char **types, char type);

#endif
