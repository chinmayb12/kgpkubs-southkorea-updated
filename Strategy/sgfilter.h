#ifndef SGFILTER_H
#define SGFILTER_H

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "gsl/gsl_math.h"
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_permutation.h"
#include "gsl/gsl_linalg.h"
#include "gsl/gsl_blas.h"

class SgFilter{
public:
	SgFilter() {}
	float smooth(std::vector<float> x, std::vector<float> y, int size, int order, int deriv);
	int factorial(int n);
	std::vector<float> sg_filter(std::vector<float> x, int order, int deriv);
	gsl_matrix *pseudoInverse(gsl_matrix *A, int n_row, int n_col);
};
#endif