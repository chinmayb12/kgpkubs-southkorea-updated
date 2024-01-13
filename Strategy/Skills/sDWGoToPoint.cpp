#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "pose.h"
#include <iostream>

#define PREDICTION_PACKET_DELAY 4
using namespace Util;

namespace Strategy
{
  void SkillSet::_dwGoToPointTrack(int botid, Pose start, Pose end, float finalvel){

    if(!algoController){
      algoController = new ControllerWrapper(&Controllers::DynamicWindow, 0, 0, PREDICTION_PACKET_DELAY);
      
    }
    int vl,vr;
    algoController->genControls(start, end, vl, vr, finalvel);
	float dist = sqrt(((start.x() - end.x())*(start.x() - end.x())) + ((start.y() - end.y())*(start.y() - end.y())));
    assert(vl <= 150 && vl >= -150);
    assert(vr <= 150 && vr >= -150);
	cout << "\n\n dist " << dist << " " << end.y() << endl;
//		if(dist < 1500.0){
//		vl = (int)(0.8*vl);
//		vr = (int)(0.8*vr);
//	}
	if(dist < 500.0){
		vl = (int)((dist/500)*vl);
		vr = (int)((dist/500)*vr);
	}
//	/*if(dist < 500.0){
//		vl = (int)(0.6*vl);
//		vr = (int)(0.6*vr);
//	}*/
////	if(dist < 250.0){
////		vl = (int)((dist/250)*vl);
////		vr = (int)((dist/250)*vr);
////	}
    comm->sendCommand(botid, vl , vr); //maybe add mutex
  }

  void SkillSet::_dwGoToPointInitTraj(int botid, Pose start, Pose end, float finalvel){
    
    if(algoController)delete algoController;
    algoController = new ControllerWrapper(&Controllers::DynamicWindow, state->homeVlVr[botID].x, state->homeVlVr[botID].y, PREDICTION_PACKET_DELAY);
  
    _dwGoToPointTrack(botid,start,end,finalvel);
  }
  
  void SkillSet::dwGoToPoint(const SParam& param)
  {
  Vector2D<int> dpoint;
    float finalvel;
    finalvel  = param.GoToPointP.finalVelocity;
	float dist = sqrt((state->ballPos.x - state->homePos[botID].x) * (state->ballPos.x - state->homePos[botID].x) + (state->ballPos.y - state->homePos[botID].y) * (state->ballPos.y - state->homePos[botID].y));
	/*if (dist < 0.5 * BOT_BALL_THRESH) {
		comm->sendCommand(botID, 0, 0);
	} else {*/
//	cout << "dw" << param.DWGoToPointP.initTraj << endl;
	  Pose start(state->homePos[botID].x, state->homePos[botID].y, state->homeAngle[botID]);
	  Pose end(param.DWGoToPointP.x, param.DWGoToPointP.y, param.DWGoToPointP.finalSlope);
	  
	  
	  if(param.DWGoToPointP.initTraj)_dwGoToPointInitTraj(botID, start, end, finalvel);
	  else _dwGoToPointTrack(botID, start, end, finalvel);
	//}
   }
}