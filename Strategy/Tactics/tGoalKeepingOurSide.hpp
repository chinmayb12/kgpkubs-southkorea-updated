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
      std::cout<<" Goalie  ::  "<<botID<<endl ;
      //cout<<"BallVel :: "<<state->ballVel.x<<std::endl; 
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
	    int dist = Vector2D<int>::dist(state->homePos[botID],state->ballPos);
      
      if (!isGoalKeeperInPosition())
      {
		  // DW was used
        std::cout<<"Assign GoToPoint"<<std::endl;
        sID = SkillSet::GoToPoint;
  //     sParam.DWGoToPointP.align = false;
        sParam.GoToPointP.finalslope =- PI / 2;
        sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.2) /*/4*/;
        sParam.GoToPointP.y = 0;
       // sParam.GoToPointP.finalVelocity = 0;
      }
      else
      {   
		  float factor = 1.3 ;
		  if(abs(state->ballVel.x) > 4000) 
			  factor = 3 ;
		  else
			  if(abs(state->ballVel.x) > 2500) 
				  factor = 2.5 ;
			  else 
				  if(abs(state->ballVel.x) > 1000)
					  factor = 2 ;
			
		// also restrict spinning when opp bot is near
		  if(dist < factor*BOT_BALL_THRESH)
		  {
			  sID = SkillSet::Spin;
			  if(state->ballPos.y > state->homePos[botID].y )
				  sParam.SpinP.radPerSec = ForwardX(-MAX_BOT_OMEGA);
			else
				  sParam.SpinP.radPerSec = ForwardX(+MAX_BOT_OMEGA) ;
		  }
		  else
		  {
			  sID = SkillSet::GoToPoint;
			  sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.2) /*/4*/;
			  int temp = getBotDestPointY();
			  if( ForwardX(state->ballPos.x )<  (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS))
			  {  sParam.GoToPointP.y = SGN(state->ballPos.y)*OUR_GOAL_MAXY ; }
			  else
			  sParam.GoToPointP.y = temp;
//			  sParam.DWGoToPointP.align = false;
			 // sParam.GoToPointP.finalVelocity = 0;
			  sParam.GoToPointP.finalslope = -PI / 2;
		  }
      }
      skillSet->executeSkill(sID, sParam);


    }

    bool isGoalKeeperInPosition()
    {
      if ((ForwardX(state->homePos[botID].x) >  (-HALF_FIELD_MAXX + GOAL_DEPTH)) &&
          (ForwardX(state->homePos[botID].x) <= (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*2)) &&
          (state->homePos[botID].y >= OUR_GOAL_MINY - DBOX_HEIGHT) &&
          (state->homePos[botID].y <= (OUR_GOAL_MAXY + DBOX_HEIGHT)))
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
  };// class TGoalKeepingOurside
} // namespace Strategy

#endif // TGOALKEEPINGOUR_HPP
