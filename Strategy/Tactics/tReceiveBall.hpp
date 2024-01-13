#ifndef TRECEIVEBALL_HPP
#define TRECEIVEBALL_HPP
#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

namespace Strategy
{
  class TReceiveBall: public Tactic
  {
  public:
    TReceiveBall(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
			iState = POSITIONING;
		} // TReceiveBall
    ~TReceiveBall()
    { } // ~TReceiveBall
		
		enum InternalState
		{
			POSITIONING,
			APPROACH,
			REATTACK,
			CHARGE
		}iState;
		
    inline bool isActiveTactic(void) const
    {
      return true;
    }
		
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv1 = -1,minv2 = *(freeBots.begin()),attacker = *(freeBots.begin());
      int mindis1 = INT_MAX,mindis2 = INT_MAX,attackerSide;
	  float dist;
	  for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
	  {
		  dist = Vector2D<int>::dist(state->homePos[*it],state->ballPos);
		  if(dist < mindis1)
		  {
			  mindis1 = dist;
			  attacker = *it;
		  }
		  
	  }
	  mindis1 = INT_MAX;
	  attackerSide = SGN(state->homePos[attacker].y);
	  for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
	  {
		  dist = state->homePos[*it].x;
		  if(SGN(state->homePos[*it].y) == -attackerSide && dist <mindis1)
		  {
			  mindis1 =dist;
			  minv1 = *it;
		  }
		  else if( dist < mindis2)
		  {
			  mindis2 = dist;
			  minv2 = *it;
		  }
	  }
	  if(minv1 == -1)
		  return minv2;
		else
			return minv1;
	
    } // chooseBestBot)
    void execute(const Param& tParam)
    {

      switch(iState)
      {
                                
        case POSITIONING:
        {
					//printf("POSITIONING\n");
					sID = SkillSet::GoToPoint;
					sParam.GoToPointP.y = -SGN(state->ballPos.y)*(HALF_FIELD_MAXY-2*BOT_RADIUS)/2;          
					sParam.GoToPointP.x = botdpointx(sParam.GoToPointP.y,sParam.GoToPointP.finalslope);
					SkillSet::addCircle(sParam.GoToPointP.x,sParam.GoToPointP.y,50, 0xFFF000);
					sParam.GoToPointP.align = false;
					char debug[100];
					sprintf(debug,"our ball %d\n",state->pr_ourBall);
					//Client::debugClient->SendMessages(debug);
					if(ball_coming())
						iState = CHARGE;
					if((/*ball_coming() || ((!state->pr_ourBall || botID==state->ourBotNearestToBall) && */(state->ballPos.y * state->homePos[botID].y > 0) && (ForwardX(state->ballPos.x) > ForwardX(state->homePos[botID].x) && (fabs(state->ballPos.y) < fabs(state->homePos[botID].y)) && ForwardX(state->ballVel.x)<0 )) || (/*(!state->pr_ourBall || botID==state->ourBotNearestToBall)&&*/Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 4*BOT_RADIUS))
					{
						iState = APPROACH;
					}
					break;
        }
        
        case APPROACH:
        {
          //printf("APPROACH\n");
          sID = SkillSet::GoToPoint;
          sParam.GoToPointP.y = state->ballPos.y;          
          sParam.GoToPointP.x = state->ballPos.x;					
					Vector2D<int>Goal(OPP_GOAL_X,0);
					sParam.GoToPointP.finalslope = Vector2D<int>::angle(Goal,state->ballPos);
          sParam.GoToPointP.align = true;
					SkillSet::addCircle(sParam.GoToPointP.x,sParam.GoToPointP.y,50, 0xFFF000);
					//ballcoming = ball in cone formed by bot and goal max/min points
          if(!((/*ball_coming() || ((!state->pr_ourBall || botID==state->ourBotNearestToBall) &&*/ (state->ballPos.y * state->homePos[botID].y > 0) && (ForwardX(state->ballPos.x) > ForwardX(state->homePos[botID].x)) && (fabs(state->ballPos.y) < fabs(state->homePos[botID].y)) && ForwardX(state->ballVel.x)<0 ) || (/*(!state->pr_ourBall || botID==state->ourBotNearestToBall) && */Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 4*BOT_RADIUS)))
          {
            iState = POSITIONING;
          }
          else if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < BOT_BALL_THRESH && ForwardX(state->ballPos.x > 0))
          {
            iState = REATTACK;
          }
          break;
        }
        
        case REATTACK:
        {
          //printf("REATTACK\n");
          sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = false;
          sParam.GoToPointP.x = OPP_GOAL_X;
          sParam.GoToPointP.y = 0;
          sParam.GoToPointP.finalslope = Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos);
          sParam.GoToPointP.increaseSpeed = 1;
          SkillSet::addCircle(sParam.GoToPointP.x,sParam.GoToPointP.y,50, 0xFFF000);
          if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) > BOT_BALL_THRESH)
          {
            iState = POSITIONING;
          }
          break;
        }

				case CHARGE:
				{
					sID = SkillSet::ChargeBall;
					if(!ball_coming())
						iState = POSITIONING;
					break;
				}
      }
        
      skillSet->executeSkill(sID,sParam);
    }
    
      float botdpointx(float &dpointy,float &slope)
    {
      int x;
      float dpointx,flag=0;

      if(state->ballVel.abs() < 200 && abs(state->ballVel.y) > 0)
      {
        if(ForwardX(state->ballPos.x) > 0 && ForwardX(state->ballPos.x) < HALF_FIELD_MAXX)
        dpointx = state->ballPos.x - 4*ForwardX(BOT_RADIUS);
        else dpointx = 0;
          
        slope=atan2((state->ballPos.y - state->homePos[botID].y),(state->ballPos.x - state->homePos[botID].x));
        
      }
      else
      {
        x =  (state->ballVel.x/ state->ballVel.y)*(dpointy - state->ballPos.y) + state->ballPos.x;
        dpointx = ForwardX(HALF_FIELD_MAXX)/2,flag=0;
				Vector2D<int>Goal(OPP_GOAL_X,0);
				slope = Vector2D<int>::angle(Goal,state->homePos[botID]);
        if(state->ballVel.y  * state->homePos[botID].y > 0)
        {
          //printf("In\n");
          if(ForwardX(x) < (3*HALF_FIELD_MAXX)/2)
            dpointx = ForwardX(2*HALF_FIELD_MAXX - BOT_RADIUS) - x, slope=atan(state->ballVel.y/state->ballVel.x),flag=1;
          if(ForwardX(x) < HALF_FIELD_MAXX)
          {
            dpointx = ForwardX(HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH);
						if(ForwardX(x) >= ForwardX(HALF_FIELD_MAXX)/2)
							dpointy = SGN(state->ballVel.y)*OUR_GOAL_MAXY;
						if(ForwardX(state->ballVel.x)>0)
							slope=atan(-state->ballVel.x/state->ballVel.y),flag=2;
          }
          if(ForwardX(x) < HALF_FIELD_MAXX/2)
          {
						dpointx = x - 4*ForwardX(BOT_RADIUS);
						if(ForwardX(state->ballVel.x)>0)
							slope=atan(-state->ballVel.x/state->ballVel.y),flag=2;
          }
          if(ForwardX(x) < 0)
            dpointx = 0 , slope=atan2((state->ballPos.y - state->homePos[botID].y),(state->ballPos.x - state->homePos[botID].x)),flag=3;
        }
        
//        if(flag==0)
//          printf("Center\n");
//          else if(flag==1)
//            printf("Reflection\n");
//            else if(flag==2)
//              printf("Predict No Reflection\n");
//              else if(flag==3)
//                printf("Near our goal\n");
      }
      if(dpointx >3 * ForwardX(HALF_FIELD_MAXX)/4 && dpointx < ForwardX(HALF_FIELD_MAXX))
        dpointx = 3 * ForwardX(HALF_FIELD_MAXX)/4;
        
        //*slope = atan2((state->ballPos.y - state->homePos[botID].y),(state->ballPos.x - state->homePos[botID].x));
        
       return dpointx;
    }

    bool ball_coming()
    {
      int ballpredx = state->ballPos.x + state->ballVel.x/30;
      int ballpredy = state->ballPos.y + state->ballVel.y/30;
      if(point_in_cone_of_bot_goal(ballpredx,ballpredy) || point_in_cone_of_bot_goal(state->ballPos.x,state->ballPos.y))
        return 1;
      return 0;
    }

    int point_in_cone_of_bot_goal(int x,int y)
    {

      int line1ball = SGN((x-state->homePos[botID].x)*(state->homePos[botID].y - (OUR_GOAL_MAXY+250)) - (y - state->homePos[botID].y)*(state->homePos[botID].x-ForwardX(HALF_FIELD_MAXX)) );
      int line2ball = SGN((x-state->homePos[botID].x)*(state->homePos[botID].y + (OUR_GOAL_MAXY+250)) - (y - state->homePos[botID].y)*(state->homePos[botID].x-ForwardX(HALF_FIELD_MAXX)) );

      x = ForwardX(HALF_FIELD_MAXX);
      y = 0;

      int line1goal = SGN((x-state->homePos[botID].x)*(state->homePos[botID].y - (OUR_GOAL_MAXY+250)) - (y - state->homePos[botID].y)*(state->homePos[botID].x-ForwardX(HALF_FIELD_MAXX)) );
      int line2goal = SGN((x-state->homePos[botID].x)*(state->homePos[botID].y + (OUR_GOAL_MAXY+250)) - (y - state->homePos[botID].y)*(state->homePos[botID].x-ForwardX(HALF_FIELD_MAXX)) );
      
      int sign;
      if(line1ball*line1goal > 0 && line2ball*line2goal > 0)
        sign = 1;
      else sign = 0;

      return ( sign );
    }
  } ;// class TReceiveBall
} // namespace Strategy
#endif // TRECEIVEBALL_HPP