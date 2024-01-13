#include "skillSet.h"
#include "pathPlanners.h"
#include "beliefState.h"
#include "config.h"
#include <math.h>
#include <stdio.h>


namespace Strategy
{
  void SkillSet::defendPoint(const SParam& param)
  {
    Vector2D<int> ballFinalpos;
    ballFinalpos.x = state->ballPos.x+0.5*state->ballVel.x;
    ballFinalpos.y = state->ballPos.y+0.5*state->ballVel.y;
      
    float point_ball_angle = atan2(ballFinalpos.y-param.GoToPointP.y,ballFinalpos.x-param.GoToPointP.x);  
    Vector2D<int> dpoint;
    //printf("Ball : %d, %d\n", state->ballPos.x, state->ballPos.y);
    dpoint.x=param.DefendPointP.x + param.DefendPointP.radius*cos( point_ball_angle);
    dpoint.y=param.DefendPointP.y + param.DefendPointP.radius*sin( point_ball_angle);
    if(dpoint.y > HALF_FIELD_MAXY)
      dpoint.y = HALF_FIELD_MAXY - 3*BOT_RADIUS;
    if(dpoint.y < -HALF_FIELD_MAXY)
      dpoint.y = -HALF_FIELD_MAXY + 3*BOT_RADIUS;
      
      
    //dpoint.y =int( ((ballFinalpos.y - state->ballPos.y)/(ballFinalpos.x - state->ballPos.x))*(dpoint.x - ballFinalpos.x) + ballFinalpos.y);
    //printf("%d %d\n",dpoint.x,dpoint.y);
    _goToPoint(botID,dpoint,0,state->homeAngle[botID],0);
  } 
}