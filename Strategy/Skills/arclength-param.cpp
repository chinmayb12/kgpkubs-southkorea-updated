#include "arclength-param.hpp"
#include <assert.h>
#include <time.h>
#include <algorithm>
#include <iostream>

namespace Integration {

double f(double u, void *integrand) {
  Integrand *in = (Integrand*) integrand;
  return (*in)(u);
}

struct func_params
{
    double s;
    Spline& p;
};

double integrate(Integrand& i, double s, double e) {
  double result, error;
  gsl_integration_workspace * w = gsl_integration_workspace_alloc (1000);
  
  gsl_function F;
  F.function = f;
  F.params = &i;
  gsl_integration_qags (&F, s, e, 0, 1e-7, 1000,
                        w, &result, &error);
  gsl_integration_workspace_free(w);
  return result;
}

const gsl_root_fdfsolver_type *T;
gsl_root_fdfsolver *sf;

void computeSplineApprox(Spline &p, alglib::spline1dinterpolant *splineSU){

    using namespace alglib;
    double n = 10; // number of points to interpolate on
    vector<double> s(n,0), u(n,0);

    for(int i=0;i<n;i++){
        u[i] = i*1.0/n;
        s[i] = integrate(p,0,u[i]);
        //std::cout << "U and S are " << u[i] << s[i] << "\n\n";
    }

    //if(ut2 < ut1){double temp=ut2;ut2=ut1;ut1=temp;}
    alglib::real_1d_array AU, AS;
    AS.setcontent(s.size(), &(s[0]));
    AU.setcontent(u.size(), &(u[0]));
    //spline1dbuildcubic(AS, AU, u.size(), 1, ut2, 1, ut1, splineSU);
    spline1dbuildcubic(AS, AU, *splineSU);

//    double ut,dut,d2ut;
//    for(int st=0;st<n;st++){
//        spline1ddiff(splineSU, 0.9*s[st], ut, dut, d2ut);
//        std::cout << "UT and ST are " << ut << 0.9*s[st] << "\n";
//    }
}

double get_ufroms_sp(double s, alglib::spline1dinterpolant splineSU){

    double u,du,d2u;
    alglib::spline1ddiff(splineSU, s, u, du, d2u);
    if(u>0.999)return 1;
    else return u;
}

double getArcLengthParam(Spline& p, double s, alglib::spline1dinterpolant *splineSU, double full, int *itr, int method) {
    // newton's method to find u for which arlength(p(0) to p(u)) = s;

  if (full < 0) {
    full = integrate(p, 0, 1);
  }

  //Using newton-rhapson technique in gsl
	//if (s < 0) s = 0; //hard code
  //assert(s >= 0);
  
  double u = get_ufroms_sp(s, *splineSU);

  double error = 1000;
  int iter = 0;
  while (fabs(error) > 1e-3 && iter < 60) {
    iter++;
    error = integrate(p,0,u)-s;
    u = u - error/p(u);
  }
  if (itr)
      *itr = iter;
  return u;
}
}
