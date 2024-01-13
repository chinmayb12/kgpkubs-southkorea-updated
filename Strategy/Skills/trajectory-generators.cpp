#include <tracking.hpp>
#include <functional>
#include <math.h>
#include "pose.h"
#include "velocity-profile.hpp"
#include "trajectory.hpp"
#include "splines.hpp"
#include "collision-checking.h"
#include <iostream>
#include <vector>

//#include "controlpoint-optimization.hpp"
using namespace std;
namespace TrajectoryGenerators {
inline Trajectory* circleGenerator(double x, double y, double r, double startTheta, double f) {
    function<double(double)> xfunc = [=](double t)->double {
        return (r*sin(2*PI*f*t + startTheta)+x)/fieldXConvert;
    };
    function<double(double)> yfunc = [=](double t)->double {
        return (r*cos(2*PI*f*t + startTheta)+y)/fieldXConvert;
    };
    return new Trajectory(xfunc, yfunc);
}

inline SplineTrajectory *cubic(Pose start, Pose end, double vls, double vrs, double vle, double vre, vector<Pose> midPoints = vector<Pose>()) {
    CubicSpline *p = new CubicSpline(start, end, midPoints);
//    p->maxk();
	bool collides_flag = true;
	double fact = 1;
	if(collides_flag == true) {
		collides_flag = false;
		using CollisionChecking::LineSegment;
		vector<LineSegment> ls;
		ls.push_back(LineSegment(-(HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert , -(HALF_FIELD_MAXY - fact*BOT_RADIUS)/fieldXConvert, (HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert, -(HALF_FIELD_MAXY - fact*BOT_RADIUS)/fieldXConvert));
		ls.push_back(LineSegment(-(HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert, (HALF_FIELD_MAXY -  fact*BOT_RADIUS)/fieldXConvert, (HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert, (HALF_FIELD_MAXY - fact*BOT_RADIUS)/fieldXConvert));
		ls.push_back(LineSegment(-(HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert, -(HALF_FIELD_MAXY - fact*BOT_RADIUS)/fieldXConvert, -(HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert, (HALF_FIELD_MAXY - fact*BOT_RADIUS)/fieldXConvert));
		ls.push_back(LineSegment((HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert, -(HALF_FIELD_MAXY -  fact*BOT_RADIUS)/fieldXConvert, (HALF_FIELD_MAXX - fact*BOT_RADIUS)/fieldXConvert, (HALF_FIELD_MAXY - fact*BOT_RADIUS)/fieldXConvert));

		for (int i = 0; i < ls.size(); i++) {
			vector<Pose> collisions = CollisionChecking::cubicSpline_LineSegmentIntersection(*p, ls[i]);
			if(collisions.size() >= 2) {
				midPoints.push_back(Pose((collisions[0].x() + collisions[1].x())/2, (collisions[0].y() + collisions[1].y())/2 , 0));  
				collides_flag = true;
				break;
			}
		}
		 CubicSpline *p = new CubicSpline(start, end, midPoints);
	}
    SplineTrajectory *st = new SplineTrajectory(p, vls, vrs, vle, vre);
    return st;
}

}
