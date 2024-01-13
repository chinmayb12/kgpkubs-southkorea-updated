#include "skillSet.h"
#include "pathPlanners.h"
#include "beliefState.h"
#include "config.h"
#include "logger.h"
#include <math.h>
#include <stdio.h>
#include "logger.h"
namespace Strategy
{
  void SkillSet::goToPoint(const SParam &param)
  {

    printf("\n\n\n\n  GOTOPOINT \n\n\n ");
    Vector2D<int> dpoint;
    float finalvel;
    dpoint.x  = param.GoToPointP.x;
    dpoint.y  = param.GoToPointP.y;
    finalvel  = param.GoToPointP.finalVelocity;
    //Util::Logger::toStdOut("Going to point : (%d , %d) from (%d, %d)",dpoint.x,dpoint.y, state->homePos[botID].x, state->homePos[botID].y);
    float dist = Vector2D<int>::dist(state->homePos[botID] , dpoint);
    if(dist < 1.1*BOT_RADIUS){
		comm->sendCommand(botID,0,0);
		return;
	} 
    if(!param.GoToPointP.align )//|| dist < 9 * BOT_BALL_THRESH * BOT_BALL_THRESH)
      _goToPoint(botID, dpoint, finalvel, param.GoToPointP.finalslope, 0,param.GoToPointP.increaseSpeed);
    else
      _goToPoint(botID, dpoint, finalvel, param.GoToPointP.finalslope, CLEARANCE_PATH_PLANNER,param.GoToPointP.increaseSpeed);
  }
}
