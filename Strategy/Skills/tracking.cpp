#include "tracking.hpp"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>

using namespace std;
double sgn(double x) {
    return (x > 0) - (0 > x);
}
Trajectory* Tracker::getTraj() {
	return traj;
}
MiscData Tracker::genControls(Pose s, int &vl, int &vr, int prevVl, int prevVr, double t) {

    Pose ref(traj->x(t)*fieldXConvert, traj->y(t)*fieldXConvert, traj->theta(t));
	std::ofstream outfile;
	outfile.open("/home/robocup/tracker.txt", std::ios_base::app);
   //outfile << s.x() << " " << ref.x() << " " << s.y() << " " << ref.y() << " " << s.theta() << " " << ref.theta()  << " " << prevVl << " " << prevVr<< endl;
	double ur1 = traj->v(t);
    double ur2 = traj->thetad(t);
    // err coordinates are in cm!
    Error err(ref, s);
    double zeta = 1, omegan = 10, g = .1;
    double k1 = 2*zeta*omegan;
    double k3 = k1;
    double k2;
    if (fabs(ur1) < 1) {
        k2 = g*fabs(ur1);
    } else {
        k2 = (omegan*omegan - ur2*ur2)/fabs(ur1);
    }
    // NOTE: hardcoding k3 = 0!
//    k3 = 0;
    // v = K.e
//    err.e3 = 0;
    double v1 = -k1*err.e1;
    double v2 = -sgn(ur1)*k2*err.e2 -k3*err.e3;
    double v = ur1*cos(err.e3) - v1;
    double w = ur2-v2;
    v/=ticksToCmS;
    vl = (v - d*w/2);
    vr = (v + d*w/2);
    // vl, vr transform now
    double vsat_ticks = vsat/ticksToCmS;
    if (vl > vsat_ticks)
        vl = vsat_ticks;
    else if (vl < -vsat_ticks)
        vl = -vsat_ticks;
    if (vr > vsat_ticks)
        vr = vsat_ticks;
    else if (vr < -vsat_ticks)
        vr = -vsat_ticks;


    // jsut for testing, calculate reference vl and vr also
    double vl_ref = (ur1/ticksToCmS - d*ur2/2);
    double vr_ref = (ur1/ticksToCmS + d*ur2/2);
	outfile  << vl << "\t" << vr << "\t" << vl_ref << "\t" << vr_ref << endl;
    return MiscData(ur1, ur2, v1, v2, t, v, w, vl, vr, vl_ref, vr_ref); //MiscData(ur1, ur2, v1, v2, t, v, w, vl, vr, vl_ref, vr_ref);
}

Pose Tracker::getNewStartPose(double t){
    double timeLMs = 16.;
    //qDebug() << traj->y(t)*fieldXConvert << "Dqa "<< endl ;
    return Pose(traj->x(t + 4*timeLMs*0.001)*fieldXConvert, traj->y(t + 4*timeLMs*0.001)*fieldXConvert, traj->theta(t + 4*timeLMs*0.001));
}

