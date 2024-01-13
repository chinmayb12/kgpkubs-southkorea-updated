#ifndef ARCLENGTHPARAM_HPP
#define ARCLENGTHPARAM_HPP
#include "trajectory.hpp"
#include <gsl/gsl_integration.h>
#include "alglib/interpolation.h"
#include "gsl/gsl_roots.h"
#include "gsl/gsl_errno.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_blas.h"

// functions for arc-length parametrizing a spline
namespace Integration {
extern gsl_integration_workspace * w;
double f(double u, void *integrand);
double integrate(Integrand& i, double s, double e);
// full = complete length of Spline from 0 to 1.
// iter: no. of iterations it takes to converge, useful for debugging/testing performance
double getArcLengthParam(Spline &p, double s, alglib::spline1dinterpolant *splineSU=NULL, double full = -1, int *iter=0, int method=0);
void computeSplineApprox(Spline &p, alglib::spline1dinterpolant *splineSU);
}
#endif // ARCLENGTHPARAM_HPP
