#ifndef reductionFUCK
#define reductionFUCK

#include "consts.h"

bool lp_objective_function_coefficients(int k, double* coefficients);
bool lp_params(int k, double **rhs_p, double **matval_p,
		double **lb_p, double **ub_p, int **matbeg_p, int **matcnt_p,
		int **matind_p, char ** sense_p, int *numCols_p, int *numRows_p,
		int ** indices_p, char ** ctypes_p);

#endif
