#ifndef TTCoverGoalPairRight_HPP
#define TTCoverGoalPairRight_HPP

#include <list>
#include "beliefState.h"
#include "tactic.h"
#include "skillSet.h"
#include "config.h"
#include "logger.h"
#include "comdef.h"

#define DEBUG 1
namespace Strategy{
class TCoverGoalPairRight : public Tactic{
public:
    int state1;
     float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
	enum{
		DEFENDING,
		BLOCKING,
		BOTH_BLOCKING,
		CLEARING,
		ATTACKING
	}iState;
    TCoverGoalPairRight(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
      state1 = 0;
            for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
	  iState = DEFENDING;
    } // TGoalKeeping

    ~TCoverGoalPairRight()
    { } // ~TGoalKeeping
    inline bool isActiveTactic(void) const
    {
      return false;
    }
   
   // change choose best  bot 
   			//Commented. Test remaining
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
		{
			int minv = *(freeBots.begin());
			int mindis = 1000000000;
			Point2D<int> goalPos(ForwardX(-(HALF_FIELD_MAXX)), 0);
			float ang1 = state->ballVel.x == 0? PI/2 :atan(state->ballVel.y/state->ballVel.x);	//KD
			for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
			{
				const int factor = 0.2;
				float perpend_dist = ForwardX(state->homePos[*it].x - ForwardX(-HALF_FIELD_MAXX));//state->home_goalpoints[2] is center of our goal
				Vector2D<int> goal;
				goal.x = OUR_GOAL_X;
				goal.y = 0;
				float dist_from_goal = Vector2D<int>::dist(state->homePos[*it], goal);
				if(dist_from_goal + factor * perpend_dist < mindis)
				{
					mindis = dist_from_goal + factor * perpend_dist;
					minv = *it;
				}
/*				if(state->ballPos.x < ForwardX(-(HALF_FIELD_MAXX-GOAL_DEPTH)*0.8) + 2*BOT_RADIUS && (state->ballPos.y >OUR_GOAL_MAXY))
				{
					botBallDist = Vector2D<int>::dist(state->ballPos, state->homePos[it]);
					if( botBallDist < botBallDist_t && state->ballPos.y < OUR_GOAL_MINY && state->ballPos.y < state->homePos[it].y && abs(state->ballPos.x - state->homePos[it].x ) <= 2*BOT_RADIUS)
					{
						botBallDist_t = botBallDist;
						it_t = it;
					}
				}	*/
			}
			return minv;
		} // chooseBestBot 
	/*
   
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
      int mindis = 1000000000;
      Point2D<int> goalPos(ForwardX(-(HALF_FIELD_MAXX)), 0);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
		const int factor = 0.2;
        float perpend_dist = ForwardX(state->homePos[*it].x - ForwardX(-HALF_FIELD_MAXX));//state->home_goalpoints[2] is center of our goal
		Vector2D<int> goal;
		goal.x = OUR_GOAL_X;
		goal.y = 0;
		float dist_from_goal = Vector2D<int>::dist(state->homePos[*it], goal);
		if(dist_from_goal + factor * perpend_dist < mindis)
        {
          mindis = dist_from_goal + factor * perpend_dist;
          minv = *it;
        }
      }
      
      return minv;
    } // chooseBestBot
*/
	
	bool isAngleSet() {
		return ((state->homeAngle[botID] >= (-PI/2 -PI/6) && state->homeAngle[botID] <= (-PI/2 + PI/6)) ||
				(state->homeAngle[botID] <= (PI/2 + PI/6) && state->homeAngle[botID] >= (PI/2 - PI/6))
				);
	}

   int nearestOppBot(int destx)
   {
	int id = 4 ;
	int minDis = HALF_FIELD_MAXX ;
    int distx ;	
 	for( int i=0;i< 5;i++)
	{   
		distx = abs(destx - state->awayPos[i].x) ; 
		if(distx < minDis)
		{
		  	minDis = distx ;
			id  = i ;
		}

	}
	   
	return id ;   
   }
   
    void execute(const Param& tParam)
    {
	int oppID ; 	
	float distBotBall = Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
	int oppInRegion[4]={0},minRegion=0,targetX = INT_MAX,targetY = 0;//
	float ang1 = state->ballVel.x == 0? PI/2 :atan(state->ballVel.y/state->ballVel.x);
	float goalAngR,goalAngL,divisor;
	int yL,yR;
	

	int predictPosY = state->ballPos.y - (state->ballPos.x - sParam.DWGoToPointP.x)*tan(ang1);
	if(predictPosY >OUR_GOAL_MAXY)
		predictPosY = OUR_GOAL_MAXY - 2*BOT_RADIUS;
	else if( predictPosY < OUR_GOAL_MINY)
		predictPosY = OUR_GOAL_MINY + 2*BOT_RADIUS;
	int goaliePredictPosY = state->ballPos.y - (state->ballPos.x + ForwardX((HALF_FIELD_MAXX-GOAL_DEPTH-BOT_RADIUS)))*tan(ang1);

	Vector2D<int> dest;
	dest.x = ForwardX(-(HALF_FIELD_MAXX-GOAL_DEPTH)*0.8);


	
	//cout<< " here"<<iState<<std::endl;
	switch (iState)
	{
		
	case DEFENDING:
		
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && abs(state->ballPos.y) < OUR_GOAL_MAXY)
		{
		   iState = BOTH_BLOCKING ;	
		   break ;
		}
		
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && (state->ballPos.y > OUR_GOAL_MAXY))
		{
		  iState = BLOCKING ;
		  break ;
		}
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x + 2*BOT_RADIUS )&& (state->ballPos.y < OUR_GOAL_MINY))
		{
		   iState = CLEARING ;
		   break ;
		}
		
		if( ForwardX(state->ballPos.x) > ForwardX(dest.x))
		{
			if(state->ballVel.x > 0 || abs(abs(ang1) - PI/2) < PI/12)
			{
				if(state->ballPos.y > OUR_GOAL_MAXY)
					dest.y = OUR_GOAL_MAXY -2*BOT_RADIUS;
				else if(state->ballPos.y < OUR_GOAL_MINY)
					dest.y = OUR_GOAL_MINY - BOT_RADIUS;
				else
					dest.y = state->ballPos.y -1*BOT_RADIUS;
			}
			else 
			{
				dest.y = predictPosY -1*BOT_RADIUS;
			}
		}
		if(ForwardX(state->ballPos.x) > ForwardX(state->homePos[botID].x )  && Vector2D<int>::dist(state->ballPos , state->homePos[botID]) < 3*BOT_RADIUS)
	      { 
			  dest.y = state->ballPos.y + SGN(state->ballPos.y)*0.5*BOT_RADIUS;
			  dest.y = state->ballPos.y ;
			  if(state->ballPos.y > OUR_GOAL_MAXY)
					dest.y = OUR_GOAL_MAXY -2*BOT_RADIUS;
				else if(state->ballPos.y < OUR_GOAL_MINY)
					dest.y = OUR_GOAL_MINY - BOT_RADIUS;
		      
		  }
		sParam.DWGoToPointP.x = dest.x;
		sParam.DWGoToPointP.y = dest.y;
		skillSet->executeSkill(SkillSet::DWGoToPoint, sParam);
		break;
		
	case CLEARING :
	 // CHANGE THE COND. FOR CLEARING .. INSTEAD OF GOTOBALL .. USE PREDICTION OR GO FROM BACK 
	    if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && abs(state->ballPos.y) < OUR_GOAL_MAXY)
		{
		   iState = BOTH_BLOCKING ;	
		   break ;
		}
		
		if(ForwardX(state->ballPos.x) > ForwardX(dest.x + 2*BOT_RADIUS))
		{
			iState = DEFENDING;
			break;
		}
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && (state->ballPos.y > OUR_GOAL_MAXY))
		{
		  iState = BLOCKING ;
		  break ;
		}
		if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x) && (abs(state->ballPos.y) - abs(state->homePos[botID].y) )< 0)
		{
		  	    
				sParam.GoToPointP.x = dest.x ;
		 		sParam.GoToPointP.y = SGN(state->ballPos.y)*(HALF_FIELD_MAXY/2 + BOT_RADIUS) ; 
				skillSet->executeSkill(SkillSet::GoToPoint, sParam);
				break;
		}
		else
		{
				sParam.GoToPointP.x = state->ballPos.x;
		 		sParam.GoToPointP.y = state->ballPos.y;
				skillSet->executeSkill(SkillSet::GoToPoint, sParam);
				break;
		
		}	

	case BLOCKING :
		
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && abs(state->ballPos.y) < OUR_GOAL_MAXY)
		{
		   iState = BOTH_BLOCKING ;	
		   break ;
		}
		
		if(ForwardX(state->ballPos.x) > ForwardX(dest.x + 2*BOT_RADIUS ))
		{
			iState = DEFENDING;
			break;
		}
		
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && (state->ballPos.y < OUR_GOAL_MINY))
		{
		   iState = CLEARING ;
		   break ;
		}
		 
		 //  oppID = 4 ; //nearestOppBot(dest.x) ;
			//std::cout<<"ID = "<<oppID<<std::endl;
			if(abs(state->awayPos[oppID].y) > OUR_GOAL_MAXY)
				sParam.DWGoToPointP.y = SGN(state->awayPos[oppID].y)*OUR_GOAL_MAXY ;
			else
				sParam.DWGoToPointP.y = state->awayPos[oppID].y ; 
			sParam.DWGoToPointP.x = dest.x;
			skillSet->executeSkill(SkillSet::DWGoToPoint, sParam);
			break; 
			
	case BOTH_BLOCKING :
	     if(ForwardX(state->ballPos.x) > ForwardX(dest.x + 2*BOT_RADIUS ))
		{
			iState = DEFENDING;
			break;
		}
		
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && (state->ballPos.y < OUR_GOAL_MINY))
		{
		   iState = CLEARING ;
		   break ;
		}
		
		if(ForwardX(state->ballPos.x) < ForwardX(dest.x) && (state->ballPos.y > OUR_GOAL_MAXY))
		{
		  iState = BLOCKING ;
		  break ;
		}
		
		oppID = nearestOppBot(dest.x) ;
			std::cout<<"ID = "<<oppID<<std::endl;
			if(state->awayPos[oppID].y < 0 )
			{
			  if(abs(state->awayPos[oppID].y) > OUR_GOAL_MAXY)
				sParam.DWGoToPointP.y = SGN(state->awayPos[oppID].y)*OUR_GOAL_MAXY ;
			  else
				sParam.DWGoToPointP.y = state->awayPos[oppID].y ;
			}
			else
			  sParam.DWGoToPointP.y = OUR_GOAL_MINY ;
		    
			sParam.DWGoToPointP.x = dest.x;
			skillSet->executeSkill(SkillSet::DWGoToPoint, sParam);
			break;
		
		
		
	}
	}


}; // class TDefense
} // namespace Strategy

#endif // TTCharge_HPP

