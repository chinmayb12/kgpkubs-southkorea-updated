#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "pose.h"
#include "trajectory-generators.cpp"
#include <iostream>
#include <fstream>

#define PREDICTION_PACKET_DELAY 4
static bool direction = true;
using namespace Util;

namespace Strategy
{
	bool SkillSet::_isFrontDirected(Pose botPos, Pose endPos, float vle, float vre) {
		int r = 10;
		double cosTheta = cos(botPos.theta());
		double sinTheta = sin(botPos.theta());
		double testx = botPos.x() + r * cosTheta;
		double testy = botPos.y() + r * sinTheta;
		double v1 = testx / tan(botPos.theta()) + testy - botPos.x() / tan(botPos.theta()) - botPos.y();
		double v2 = endPos.x() / tan(botPos.theta()) + endPos.y() - botPos.x() / tan(botPos.theta()) - botPos.y();
		bool direct = true;
		if((v1 * v2) >= 0) 
			direct = true;
		else
			direct = false;
			
		if((abs(botPos.x()) > (HALF_FIELD_MAXX - 2*BOT_RADIUS)) && 	(abs(endPos.x()) > abs(botPos.x())))
			direct = !direct;
		else if((abs(botPos.y()) > (HALF_FIELD_MAXY - 2*BOT_RADIUS)) && 	(abs(endPos.y()) > abs(botPos.y())))
			direct = !direct;
		return direct;	
	}	
	void SkillSet::_splineGoToPointTrack(int botid, Pose start, Pose end, float finalvel, float vls, float vrs){
		
		counter++;	
		if(!algoController){
			if(traj)
				algoController = new ControllerWrapper(traj, vls, vrs, PREDICTION_PACKET_DELAY);
		}
		int vl,vr;
		if(!direction)
			start.setTheta(normalizeAngle(start.theta() - PI));
			
		algoController->genControls(start, end, vl, vr, finalvel);
		assert(vl <= 180 && vl >= -180);
		assert(vr <= 180 && vr >= -180);
		if (direction){
			comm->sendCommand(botid, vl, vr); //maybe add mutex
		}else {
			//cout << "ulta chaloooo" << endl;
			int vl1 = (-1)*vr;
			int vr1 = (-1)*vl;
			comm->sendCommand(botid, vl1, vr1);
		}
	}

	void SkillSet::_splineGoToPointInitTraj(int botid, Pose start, Pose end, float finalvel, float vls, float vrs, int flag){
	
		counter = 0;
		Pose start2(state->homePos[botID].x, state->homePos[botID].y, normalizeAngle(state->homeAngle[botID] - PI));
		Vector2D<float> delayedVel(vls, vrs);
		direction = _isFrontDirected(start, end, vls, vrs);
		
		if(direction){
			if(!flag){
				if(traj)
					delete traj;
				traj = TrajectoryGenerators::cubic(start, end, vls, vrs, 0, 0);
			}
			else{
			//	cout << "here" << endl;
				delayedVel = algoController->getDelayedVel();
				start = algoController->getNewStartPose();
				if(traj) 
					delete traj;
				traj = TrajectoryGenerators::cubic(start, end, delayedVel.x , delayedVel.y , 0, 0);
			}
		}
		else {
			if(!flag){
				if(traj)
					delete traj;
				traj = TrajectoryGenerators::cubic(start2, end, vls, vrs, 0, 0);
			}
			else{
			//	cout << "here" << endl;
				delayedVel = algoController->getDelayedVel();
				Pose start2 = algoController->getNewStartPose();
				if(traj)
					delete traj;
				traj = TrajectoryGenerators::cubic(start2, end, delayedVel.x, delayedVel.y, 0, 0);
			}
		}
		
		if(algoController)
			delete algoController;
		
		if(direction)
			algoController = new ControllerWrapper(traj, state->homeSentVlVr[botID].x, state->homeSentVlVr[botID].y, PREDICTION_PACKET_DELAY);
		else
			algoController = new ControllerWrapper(traj, (-1)*state->homeSentVlVr[botID].y, (-1)*state->homeSentVlVr[botID].x, PREDICTION_PACKET_DELAY);
		
		//algoController = new ControllerWrapper(traj, delayedVel.x , delayedVel.y , PREDICTION_PACKET_DELAY);

		_splineGoToPointTrack(botid,start,end,finalvel, vls, vrs);
	}
	
  void SkillSet::splineGoToPoint(const SParam& param)
  {
	Vector2D<int> dpoint;
    float finalvel;
    finalvel  = param.GoToPointP.finalVelocity;
	Pose start(state->homePos[botID].x, state->homePos[botID].y, state->homeAngle[botID]);
	Pose end(param.SplineGoToPointP.x, param.SplineGoToPointP.y, param.SplineGoToPointP.finalSlope);
	
	cout << state->homePos[botID].x << " " << state->homePos[botID].y << endl;
	if(param.SplineGoToPointP.initTraj == 1){
		_splineGoToPointInitTraj(botID, start, end, finalvel, state->homeSentVlVr[botID].x, state->homeSentVlVr[botID].y, 0);
	}
	else if(counter > 30){
		_splineGoToPointInitTraj(botID, start, end, finalvel, state->homeSentVlVr[botID].x, state->homeSentVlVr[botID].y, 1);
	}
	else 
		_splineGoToPointTrack(botID, start, end, finalvel, state->homeSentVlVr[botID].x, state->homeSentVlVr[botID].y);
   }
}

