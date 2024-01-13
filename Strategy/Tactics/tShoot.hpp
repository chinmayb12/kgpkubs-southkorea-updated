#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "geometry.hpp"
#include <algorithm>
#include <fstream>
namespace Strategy
{
  class TShoot : public Tactic
  {
	  public:
    TShoot(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TShoot
    ~TShoot()
    { } // ~TShoot
    inline bool isActiveTactic(void) const
    {
      return true;
    }
	int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
	  
	  static int counter = 0 ;
	  assert(tParam != 0);
      int minv   = *freeBots.begin();
      int mindis = INT_MAX;
      Vector2D<int> tGoToPoint(tParam->PositionP.x, tParam->PositionP.y);

      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
		     //   printf("freeBots :: %d \n ",*it);
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_point = Vector2D<int>::dist(state->homePos[*it], state->ballPos);//(state->homePos[*it] - state->ballPos).absSq();
		
		//printf(" dis_from_point %d is ::  %d %d \n ",*it,state->ballPos.x, state->ballPos.y);
		//printf("dist_from_ball %d :: %f \n",*it,dis_from_point) ;
        if(dis_from_point < mindis)
        {
          mindis = dis_from_point;
		  
          minv = *it;
        }
      }
     // printf(" \n \n %d assigned Position\n \n ", minv);
	 if(counter==2)
		//  assert(tParam=0);
	  counter++ ;
     // return minv;
	 return minv ;
    } // chooseBestBot
	
	Vector2D<int> getOppGoalie(){
		int id=0;
		Vector2D<int> oppGoalMid(ForwardX(HALF_FIELD_MAXX), 0);
		float dist2;
		float dist1=Vector2D<int>::dist(state->awayPos[0], oppGoalMid);
		for(int i=1;i<5;i++){
			dist2=Vector2D<int>::dist(state->awayPos[i], oppGoalMid);
			if(dist2<dist1){
				id=i;
				dist1=dist2;
			}
		}
		return state->awayPos[id];	
	}
	
    void execute(const Param& tParam)
    {
	
	  float destangle=Vector2D<int>::angle(state->homePos[botID],state->ballPos); 
	  sID=SkillSet::GoToPoint;
	  float m=atan2(state->ballPos.y-state->homePos[botID].y,state->ballPos.x-state->homePos[botID].x);
	  Vector2D<int> dest;
	  if(getOppGoalie().y>0)
		  dest.y=OPP_GOAL_MINY + 0.5*BOT_RADIUS;
	  else dest.y=OPP_GOAL_MAXY - 0.5*BOT_RADIUS;
	  dest.x=ForwardX(HALF_FIELD_MAXX);
	  
	  sParam.GoToPointP.x=state->ballPos.x;
	  sParam.GoToPointP.y=state->ballPos.y;
	  cout<<state->ballPos.x<<" "<<state->ballPos.y<<endl ; 
	  if(Vector2D<int>::dist(state->homePos[botID],state->ballPos)<1.5*BOT_BALL_THRESH)
	  {
		sParam.GoToPointP.x=dest.x;
	    sParam.GoToPointP.y=dest.y;
		//sParam.GoToPointP.finalslope=Vector2D<int>::angle(dest,state->ballPos);
	  }
	  sParam.GoToPointP.finalslope=Vector2D<int>::angle(dest,state->ballPos);
	  //sParam.GoToPointP.finalslope = destangle ; 
	  
	  
	  skillSet->executeSkill(sID,sParam);
	}  
  };
}
// we r using this below
/*
#ifndef TSHOOT_HPP
#define TSHOOT_HPP
#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "geometry.hpp"
#include <algorithm>
#include <fstream>
namespace Strategy
{
  class TShoot : public Tactic
  {
  public:
    TShoot(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TShoot
    ~TShoot()
    { } // ~TShoot
    inline bool isActiveTactic(void) const
    {
      return true;
    }
	int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
	  
	  static int counter = 0 ;
	  assert(tParam != 0);
      int minv   = *freeBots.begin();
      int mindis = 1000000000;
      Vector2D<int> tGoToPoint(tParam->PositionP.x, tParam->PositionP.y);

      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
		     //   printf("freeBots :: %d \n ",*it);
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_point = Vector2D<int>::dist(state->homePos[*it], state->ballPos);//(state->homePos[*it] - state->ballPos).absSq();
		
		//printf(" dis_from_point %d is ::  %d %d \n ",*it,state->ballPos.x, state->ballPos.y);
		//printf("dist_from_ball %d :: %f \n",*it,dis_from_point) ;
        if(dis_from_point < mindis)
        {
          mindis = dis_from_point;
		  
          minv = *it;
        }
      }
     // printf(" \n \n %d assigned Position\n \n ", minv);
	 if(counter==2)
		//  assert(tParam=0);
	  counter++ ;
     // return minv;
	 return minv ;
    } // chooseBestBot
	
	Vector2D<int> getOppGoalie(){
		int id=0;
		Vector2D<int> oppGoalMid(ForwardX(HALF_FIELD_MAXX), 0);
		float dist2;
		float dist1=Vector2D<int>::dist(state->awayPos[0], oppGoalMid);
		for(int i=1;i<5;i++){
			dist2=Vector2D<int>::dist(state->awayPos[i], oppGoalMid);
			if(dist2<dist1){
				id=i;
				dist1=dist2;
			}
		}
		return state->awayPos[id];	
	}
	
    void execute(const Param& tParam)
    {
	
	 printf("Shoot Bot ID%d \n",botID); 
      float tmp_angle1, tmp_angle2, tmp_angle3;
      float angle1, angle2, finalSlope, finalAngle;
      tmp_angle1 = Vector2D<int>::angle(Vector2D<int>(ForwardX(HALF_FIELD_MAXX), OPP_GOAL_MAXY), state->ballPos);
      tmp_angle2 = Vector2D<int>::angle(getOppGoalie(), state->ballPos);
      tmp_angle3 = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX, OPP_GOAL_MINY), state->ballPos);
      angle1 = fabs(tmp_angle2 - tmp_angle1);
      angle2 = fabs(tmp_angle3 - tmp_angle2);
      if(angle1 > angle2)
        finalSlope = (tmp_angle2 + tmp_angle1) / 2;
      else
        finalSlope = (tmp_angle2 + tmp_angle3) / 2;
		
      Util::Logger::toStdOut("Final Angle : %f\n", finalSlope);
      float dist_from_ball = Vector2D<int>::dist(state->homePos[botID], state->ballPos);
      int finalx, finaly;
      if(dist_from_ball < 2*BOT_BALL_THRESH)
      {
        finalx = HALF_FIELD_MAXX;
       // /* The point of intersection with the goal Vertical line y = mx1 + (y2 - mx2)
        finaly = finalSlope * HALF_FIELD_MAXX + (state->homePos[botID].y - finalSlope * state->homePos[botID].x);
		finalAngle=finalSlope;
      }
      else
      {
        finalx = state->ballPos.x;
        finaly = state->ballPos.y;
		finalAngle=0;
      }
		sID = SkillSet::GoToPoint;
		sParam.GoToPointP.align = false;
		sParam.GoToPointP.x = finalx;
		sParam.GoToPointP.y = finaly;
		sParam.GoToPointP.finalslope=finalAngle;
		sParam.GoToPointP.increaseSpeed = 1;
		skillSet->executeSkill(sID, sParam);
	}
  }; // class TShoot
} // namespace Strategy
#endif // TSHOOT_HPP
*/
//****************************************************************
/*
#ifndef TSHOOT_HPP
#define TSHOOT_HPP
#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
namespace Strategy
{
  class TShoot : public Tactic
  {
  public:
    TShoot(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TShoot
    ~TShoot()
    { } // ~TShoot
    inline bool isActiveTactic(void) const
    {
      return true;
    }
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10000;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = (state->homePos[*it] - state->ballPos).absSq();
        if(dis_from_ball < mindis)
        {
          mindis = dis_from_ball;
          minv = *it;
        }
      }
      //return minv;
	  return state->ourBotNearestToBall;
    } // chooseBestBot
    void execute(const Param& tParam)
    {
      printf("Shoot BotID: %d\n",botID);
      
      float tmp_angle1, tmp_angle2, tmp_angle3;
      float angle1, angle2, finalSlope;
      tmp_angle1 = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX, OPP_GOAL_MAXY), state->ballPos);
      tmp_angle2 = Vector2D<int>::angle(getOppGoalie(), state->ballPos);
      tmp_angle3 = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX, OPP_GOAL_MINY), state->ballPos);
      angle1 = fabs(tmp_angle2 - tmp_angle1);
      angle2 = fabs(tmp_angle3 - tmp_angle2);
      if(angle1 > angle2)
        finalSlope = (tmp_angle2 + tmp_angle1) / 2;
      else
        finalSlope = (tmp_angle2 + tmp_angle3) / 2;
      Util::Logger::toStdOut("Final Angle : %f\n", finalSlope);
      float dist_from_ball = Vector2D<int>::dist(state->homePos[botID], state->ballPos);
      int finalx, finaly;
      if(dist_from_ball < BOT_BALL_THRESH)
      {
        finalx = HALF_FIELD_MAXX;
        // The point of intersection with the goal Vertical line y = mx1 + (y2 - mx2)
        finaly = finalSlope * HALF_FIELD_MAXX + (state->homePos[botID].y - finalSlope * state->homePos[botID].x);
      }
      else
      {
        finalx = state->ballPos.x;
        finaly = state->ballPos.y;
      }
      gotoPointExact(finalx, finaly, true, finalSlope, 0);
    }
    Vector2D<int> getOppGoalie()
    {
      return state->awayPos[0];
    }
  }; // class TShoot
} // namespace Strategy
#endif // TSHOOT_HPP
*/