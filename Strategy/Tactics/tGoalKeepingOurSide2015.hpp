#ifndef TGOALKEEPINGOUR_HPP
#define TGOALKEEPINGOUR_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "geometry.hpp"
#include <algorithm>

namespace Strategy
{
  class TGoalKeepingOurSide : public Tactic
  {
  public:
     float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
    TGoalKeepingOurSide(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
            for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
    } // TGoalKeeping

    ~TGoalKeepingOurSide()
    { } // ~TGoalKeeping
    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    { 
	  //sleep(3);
	  static int counter = 0 ;
      int minv = *(freeBots.begin());
      int mindis = 1000000000;
      Point2D<int> goalPos(ForwardX(-(HALF_FIELD_MAXX)), 0);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
                const int factor = 0.2;
        float perpend_dist = ForwardX(state->homePos[*it].x - ForwardX(-HALF_FIELD_MAXX));
                Vector2D<int> goal;
                goal.x = OUR_GOAL_X;
                goal.y = 0;
                float dist_from_goal = Vector2D<int>::dist(state->homePos[*it],goal) ;  
               
				
                if(dist_from_goal + factor * perpend_dist < mindis)
        {
          mindis = dist_from_goal + factor * perpend_dist;
          minv = *it;
        }
      }
	  //printf(" :: %d ::",minv);
	//if(counter==2)
      //assert(tParam=0);
	//counter++;
	return minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      
      printf("Goalie Bot ID%d \n",botID);
      movementError[movementErrorIndex++] = (Vector2D<int>::distSq(prevBotPos, state->homePos[botID])) + (prevBotAngle - state->homeAngle[botID])*(prevBotAngle - state->homeAngle[botID])*50000;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      movementErrorIndex %= 10;
      movementErrorSum = 0;
      for(int i=0; i<10; i++)
        movementErrorSum += movementError[i];
	  
	  	  int dist = Vector2D<int>::dist(state->homePos[botID],state->ballPos);
		  
	float factorx = 0.00008;
	if( state->ballVel.x<200 )
		factorx=0.000012;
	else if(state->ballVel.x<500 )
		factorx=0.000015;
	else if(state->ballVel.x<800 )
		factorx=0.000018;
	else if(state->ballVel.x<1200 )
		factorx=0.000026;
	else if(state->ballVel.x<1400)
		factorx=0.000045;
	else if(state->ballVel.x<1800 )
		factorx=0.000065;
	else if(state->ballVel.x<2200 )
		factorx=0.000090;
	else if(state->ballVel.x<2600 )
		factorx=0.000130;
	else if(state->ballVel.x<3000 )
		factorx=0.000150;
	else if(state->ballVel.x<3400 )
		factorx=0.000160;
	else if(state->ballVel.x<3800 )
		factorx=0.000180;
	else factorx=0.0002;


	float factory = 0.00008;
	if( state->ballVel.y<200 )
		factory=0.000012;
	else if(state->ballVel.y<500 )
		factory=0.000015;
	else if(state->ballVel.y<800 )
		factory=0.000018;
	else if(state->ballVel.y<1200 )
		factory=0.000026;
	else if(state->ballVel.y<1400)
		factory=0.000045;
	else if(state->ballVel.y<1800 )
		factory=0.000065;
	else if(state->ballVel.y<2200 )
		factory=0.000090;
	else if(state->ballVel.y<2600 )
		factory=0.000130;
	else if(state->ballVel.y<3000 )
		factory=0.000150;
	else if(state->ballVel.y<3400 )
		factory=0.000160;
	else if(state->ballVel.y<3800 )
		factory=0.000180;
	else factory=0.00020;

	ballFinalpos.x = state->ballPos.y+ state->ballVel.y*dist*factory;
	ballFinalpos.y = state->ballPos.x+ state->ballVel.x*dist*factorx+BOT_RADIUS*2;
*/
      
	  if( ballInDangerBarPos())
		{
			if(isGoalKeeperInPosition() && dist <= BOT_BALL_THRESH)
			{		//By KD
				sID = SkillSet::Spin ;
				if(state->ballPos.y > state->homePos[botID].y)
					sParam.SpinP.radPerSec = -MAX_BOT_OMEGA ;
				else
					sParam.SpinP.radPerSec = MAX_BOT_OMEGA ;
				skillSet->executeSkill(sID, sParam);
				return;	
			}
			else
			{
				sID = SkillSet::GoToPoint;
				sParam.GoToPointP.align = false;
				sParam.GoToPointP.finalslope = -SGN(state->ballPos.y)*PI/4;
				sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH - BOT_RADIUS) /*/4*/;
				sParam.GoToPointP.y = ForwardX(SGN(state->ballPos.y)*(OUR_GOAL_MAXY - BOT_RADIUS));
				sParam.GoToPointP.finalVelocity = dist*0.25;
			}
		}
      else if (!isGoalKeeperInPosition() )
      {
        sID = SkillSet::GoToPoint;
        sParam.GoToPointP.align = false;
        sParam.GoToPointP.finalslope =- PI / 2;
        sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.2) /*/4*/;
        sParam.GoToPointP.y = predictYForGoalKeeper(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.2);	//0;
        sParam.GoToPointP.finalVelocity = 0;

      }
      else
      {
		  if(dist < 1.5*BOT_BALL_THRESH)
		  {
			  sID = SkillSet::Spin;
			  if(state->ballPos.y < 0 )
				  sParam.SpinP.radPerSec = -MAX_BOT_OMEGA   ;
			else
				  sParam.SpinP.radPerSec = +MAX_BOT_OMEGA   ;
		  }
		  else
		  {
			  sID = SkillSet::DWGoToPoint;
			  sParam.DWGoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.2) /*/4*/;
			  int temp = getBotDestPointY();
			  if(abs(state->ballPos.x - state->homePos[botID].x) < ForwardX(HALF_FIELD_MAXX)/4)
				  sParam.DWGoToPointP.y = predictYForGoalKeeper(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.2); 	//temp;
			  else sParam.DWGoToPointP.y = state->ballPos.y;
			  sParam.DWGoToPointP.align = false;
			  sParam.DWGoToPointP.finalVelocity = 0;
			  sParam.DWGoToPointP.finalslope = -PI / 2;
		  }
      }
      
      skillSet->executeSkill(sID, sParam);


    }

    bool isGoalKeeperInPosition()
    {
      if (((ForwardX(state->homePos[botID].x) >  (-HALF_FIELD_MAXX + GOAL_DEPTH)) &&
          (ForwardX(state->homePos[botID].x) <= (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*2)) &&
          (state->homePos[botID].y >= OUR_GOAL_MINY - DBOX_HEIGHT) &&
          (state->homePos[botID].y <= (OUR_GOAL_MAXY + DBOX_HEIGHT)))
			|| (state->ballPos.x < ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS) &&
				abs(state->ballPos.y) <= OUR_GOAL_MAXY + 1.5*BOT_RADIUS) &&
				state->homePos[botID].x < ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH))
        return true;
      else
        return false;

    }
    
    int getBotDestPointY()
    {
      Vector2D<int> ballFinalpos, botDestination, point;
      int flag=2;
      
      float balldistratio = fabs(state->ballPos.x)/(HALF_FIELD_MAXX-DBOX_WIDTH)<1 ? fabs(state->ballPos.x)/(HALF_FIELD_MAXX-DBOX_WIDTH):1 ;
      
      point.y = balldistratio*SGN(state->ballPos.y)*MIN(fabs(state->ballPos.y), OUR_GOAL_MAXY); 
    
   //    Workaround for ball velocity 0
      if( ( ( fabs(state->ballVel.y) + fabs(state->ballVel.x) < 100) ) || (ForwardX(state->ballVel.x)<0 && ForwardX(state->ballVel.x)>(-100)) )
      {
       if(ForwardX(state->ballPos.x) > ( -HALF_FIELD_MAXX*0.3))
        point.y = 0,flag=0;
      }
      else if(ForwardX(state->ballVel.x)>0 )
      {
        if(ForwardX(state->ballPos.x) > (-HALF_FIELD_MAXX*0.3))
          point.y = 0,flag =0;
      }
      else if (ForwardX(state->ballVel.x) <=(-50) )
      {
        if(ForwardX(state->ballPos.x) > (-HALF_FIELD_MAXX*0.8) )
        point.y = (state->ballVel.y/state->ballVel.x)*(ForwardX(-HALF_FIELD_MAXX+ GOAL_DEPTH + BOT_RADIUS*1.5) - (state->ballPos.x)) + state->ballPos.y,flag = 1;
      }

      /* Set Limits on y to not exceed DBOX Y Limits*/
      if(point.y < OUR_GOAL_MINY + BOT_RADIUS)
       {
         if(point.y >-HALF_FIELD_MAXY)
         point.y = OUR_GOAL_MINY + BOT_RADIUS;
        else
          point.y = 0;
       }
       
      else if(point.y > OUR_GOAL_MAXY - BOT_RADIUS)
      {
         if(point.y < HALF_FIELD_MAXY)
          point.y = OUR_GOAL_MAXY - BOT_RADIUS;
          else
          point.y = 0;
       }
	   

          
      return point.y;
    }
	
	static bool ballInDangerBarPos()
	{				//By KD
		if(state->ballPos.x < ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS &&
			abs(state->ballPos.y) <= OUR_GOAL_MAXY + 1.5*BOT_RADIUS &&
			abs(state->ballPos.y) >= OUR_GOAL_MAXY - 1.5*BOT_RADIUS)
			return true;
		else
			return false;
	}
	int predictYForGoalKeeper(int inputX)
	{				//By KD
		float m = (state->ballVel.x < 10) ? 0: state->ballVel.y/state->ballVel.x;
		float c = (state->ballPos.y - m*state->ballPos.x);
		float predictY = m*(inputX + BOT_RADIUS) + c;
		int i = 0;
		while( abs(predictY > HALF_FIELD_MAXY) && i < 3)
		{
			predictY = SGN(predicty)*(2*HALF_FIELD_MAXY - abs(predictY));
			++i;
		}
		if( i == 2)
			return 0;
		if( predictY > OUR_GOAL_MAXY)
			return OUR_GOAL_MAXY;
		if( predictY < OUR_GOAL_MAXY)
			return OUR_GOAL_MINY;
		return predictY;
	}
  };// class TGoalKeepingOurside
} // namespace Strategy

#endif // TGOALKEEPINGOUR_HPP