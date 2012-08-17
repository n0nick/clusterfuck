#ifndef reductionFUCK
#define reductionFUCK

#include "consts.h"

bool lp_objective_function_coefficients(int k, double** coefficients);
bool lp_rhs_sense(int k, double** rhs, char** sense);
bool lp_matrix(int k, int *matbeg, int *matcnt, int *matind, double *matval);

#endif
