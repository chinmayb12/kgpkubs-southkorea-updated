#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "pose.h"
#include <iostream>
#include "ballinterception.cpp"

#define PREDICTION_PACKET_DELAY 4
using namespace Util;
static bool direction2 = true;
static bool lastDirection2 = true;

namespace Strategy
{
  void SkillSet::_splineInterceptBallTrack(int botid, Pose start, Vector2D<float> ballPos, Vector2D<float> ballVel, Vector2D<float> botVel, float final_vl, float final_vr){
    
    interceptCounter++;
    if(!algoController){
      if(traj)
        algoController = new ControllerWrapper(traj, state->homeSentVlVr[botID].x, state->homeSentVlVr[botID].y, PREDICTION_PACKET_DELAY);
    }
    
    if(!direction2)
      start.setTheta(normalizeAngle(start.theta() - PI));
    
    int vl,vr;
    Pose dummy(0,0,0);
    algoController->genControls(start, dummy, vl, vr, 0);
    predictedPoseQ.push_back(algoController->getPredictedPose(start));
    predictedPoseQ.pop_front();
    assert(vl <= 150 && vl >= -150);
    assert(vr <= 150 && vr >= -150);
	myfileX << vl << " " << vr << endl;

    if (direction2)
      comm->sendCommand(botid, vl, vr); //maybe add mutex
    else {
      int vl1 = (-1)*vr;
      int vr1 = (-1)*vl;
      comm->sendCommand(botid, vl1, vr1);
    }
  }

  void SkillSet::_splineInterceptBallInitTraj(int botid, Pose start, Vector2D<float> ballPos, Vector2D<float> ballVel, Vector2D<float> botVel, float final_vl, float final_vr, int flag){

    float dist = Vector2D<int>::dist(state->homePos[botID], state->ballPos);  
    interceptCounter = 0; 
//    if(dist < 150){
//     comm->sendCommand(botID, 0, 0);
//      return;
//   }
    Pose start2(state->homePos[botID].x, state->homePos[botID].y, normalizeAngle(state->homeAngle[botID] - PI));
    Vector2D<int> GoalPoint(ForwardX(HALF_FIELD_MAXX), 0);
    direction2 = _isFrontDirected(start, Pose(state->ballPos.x, state->ballPos.y , Vector2D<int>::angle(state->ballPos, GoalPoint)), state->homeSentVlVr[botID].x, state->homeSentVlVr[botID].y);
    interceptCounter = 0; 
    Vector2D<float> genVel, delayedVel; Pose startP;
	Vector2D<int> prevV;
	if(traj)delete traj;
	
	if(!flag){
		prevV.x=0;prevV.y=0;
		if(direction2){
			genVel.x = botVel.x; genVel.y = botVel.y;
			startP = start;
		}
		else{
			genVel.x = (-1)*botVel.y; genVel.y = (-1)*botVel.x;
			startP = start2;
		}
	}
	else{
		prevV = algoController->getPrevDelVel();
		if(lastDirection2 == direction2){
			genVel = algoController->getDelayedVel();
			startP = predictedPoseQ.back();
		}
		else{
		  delayedVel = algoController->getDelayedVel();
		  genVel.x = (-1)*delayedVel.y; genVel.y = (-1)*delayedVel.x;
          startP = predictedPoseQ.back();
          startP.setTheta(normalizeAngle(start.theta() - PI));
		}
	}
	traj = BallInterception::getIntTraj(startP, ballPos, ballVel, genVel, state->ballAcc);	
	algoController = new ControllerWrapper(traj, genVel.x, genVel.y, PREDICTION_PACKET_DELAY, prevV.x, prevV.y);
	lastDirection2 = direction2 ; 
	
	for (int i = 0; i < PREDICTION_PACKET_DELAY; i++) {
        predictedPoseQ.push_back(Pose(state->homePos[botID].x, state->homePos[botID].y, state->homeAngle[botID]));
    }	
    _splineInterceptBallTrack(botID, start, ballPos, ballVel, botVel, final_vl, final_vr);
  }
  
  void SkillSet::splineInterceptBall(const SParam& param)
  {
    float final_vl = param.SplineInterceptBallP.vl;
    float final_vr = param.SplineInterceptBallP.vr;
    Vector2D<float> ballPos, ballVel, botVel;
    Pose start(state->homePos[botID].x, state->homePos[botID].y, state->homeAngle[botID]);
      
    ballPos.x = state->ballPos.x;
    ballPos.y = state->ballPos.y;
    if(param.SplineInterceptBallP.velGiven == 0){
      ballVel.x = state->ballVel.x;
      ballVel.y = state->ballVel.y;
    }
    else{
      ballVel.x = param.SplineInterceptBallP.ballVelX;
      ballVel.y = param.SplineInterceptBallP.ballVelY;
    }
    botVel.x = state->homeSentVlVr[botID].x;
    botVel.y = state->homeSentVlVr[botID].y;
    double dt = 10;
	float deviatedDist;
	if((traj) && (algoController)){
			SplineTrajectory *st = dynamic_cast<SplineTrajectory*>(traj);
			dt = st->totalTime() - algoController->getCurrentTimeS();
			if(sTrack < 3){
				Pose refPose = algoController->getReferencePose();
				deviatedDist = sqrt((refPose.x() - start.x())*(refPose.x() - start.x()) + (refPose.y() - start.y())*(refPose.y() - start.y()));
				splineTrack = algoController->ballPredictionCheck(Vector2D<float>(refPose.x(), refPose.y()), ballPos, ballVel, deviatedDist, st->totalTime());
				if(!splineTrack){
					sTrack++;	
				}
				else{
					sTrack = 0;
				}
			}
	}

    if((dt < 0.075 || deviatedDist > 3*BOT_RADIUS || sTrack >=3) && sCount < 2 ){
		sCount++;
		comm->sendCommand(botID, 0, 0);
		cout<<"Deviating"<<endl;
	}
//	else if(param.SplineInterceptBallP.initTraj == 1 || dt < 0.075){
	else if(param.SplineInterceptBallP.initTraj == 1 || deviatedDist > 3*BOT_RADIUS || dt < 0.075 || sTrack >=3){// || (!traj)){
		sCount = 0;
		sTrack = 0;
       _splineInterceptBallInitTraj(botID, start, ballPos, ballVel, botVel, final_vl, final_vr, 0);
	}
 //   else if (interceptCounter > 80 || dt < 0.075) //param.SplineInterceptBallP.changeSpline == 1 &&
 //      _splineInterceptBallInitTraj(botID, start, ballPos, ballVel, botVel, final_vl, final_vr, 1);
    else {
		sCount = 0;
		sTrack = 0;
        _splineInterceptBallTrack(botID, start, ballPos, ballVel, botVel, final_vl, final_vr);
    }
  }
}