#ifndef TTCoverGoal_HPP
#define TTCoverGoal_HPP

#include <list>
#include "beliefState.h"
#include "tactic.h"
#include "skillSet.h"
#include "config.h"
#include "logger.h"
#include "comdef.h"

#define DEBUG 1
namespace Strategy{
class TCoverGoal : public Tactic{
public:
    int state1;
     float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
    TCoverGoal(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
      state1 = 0;
            for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
    } // TGoalKeeping

    ~TCoverGoal()
    { } // ~TGoalKeeping
    inline bool isActiveTactic(void) const
    {
      return false;
    }

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

	
	bool isAngleSet() {
		return ((state->homeAngle[botID] >= (-PI/2 -PI/6) && state->homeAngle[botID] <= (-PI/2 + PI/6)) ||
				(state->homeAngle[botID] <= (PI/2 + PI/6) && state->homeAngle[botID] >= (PI/2 - PI/6))
				);
	}


    void execute(const Param& tParam)
    {
      printf("CoverGoal BotID: %d\n",botID);
      
      movementError[movementErrorIndex++] = (Vector2D<int>::distSq(prevBotPos, state->homePos[botID])) + (prevBotAngle - state->homeAngle[botID])*(prevBotAngle - state->homeAngle[botID])*50000;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      movementErrorIndex %= 10;
      movementErrorSum = 0;
      for(int i=0; i<10; i++)
        movementErrorSum += movementError[i];
      /*if(movementErrorSum < 500 && tParam.AttackP.rotateOnError)
      {
        sID = SkillSet::Spin;
        sParam.SpinP.radPerSec = MAX_BOT_OMEGA * (state->homePos[botID].y > 0? ForwardX(1): ForwardX(-1));
        skillSet->executeSkill(sID, sParam);
        return;
      }*/
   //   Util::Logger::toStdOut("bot vel %f, %f\n",  state->homeVel[botID].x,state->homeVel[botID].y);
      if (!isGoalKeeperInPosition() )
      {
        //Bot elsewhere in field. Bring the bot to the Goalkeeper position.
        //  Util::Logger::toStdOut("Bot going to goalkeeper positon.");
        sID = SkillSet::DWGoToPoint;
        sParam.DWGoToPointP.finalSlope =- PI / 2;
        sParam.DWGoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + 3.5*BOT_RADIUS);
        sParam.DWGoToPointP.y = 0;

      }
      else if (!isAngleSet()) {
			sID = SkillSet::TurnToAngle;
			sParam.TurnToAngleP.finalslope = -PI / 2;
			skillSet->executeSkill(sID, sParam);
			return;
		}
      {
        //Bot in Goalkeeper position. Show goalkeeping skills.
        //note: try commenting this if block if bot not working properly
       /* if(fabs(fabs(state->homeAngle[botID]) - PI / 2) > DRIBBLER_BALL_ANGLE_RANGE / 2 && fabs(state->homeVel[botID].y)<150 && fabs(state->homePos[botID].y-getBotDestPointY())<2*BOT_POINT_THRESH)
        {
          sID = SkillSet::TurnToAngle;
          sParam.TurnToAngleP.finalslope =- PI / 2;
          state1 = 0;
          printf("Turning Situation\n");
        }
         */
//        else if (fabs(fabs(state->homeAngle[botID]) - PI / 2) > 2 * DRIBBLER_BALL_ANGLE_RANGE && state1 == 1)
//        {
//          sID = SkillSet::TurnToAngle;
//          sParam.TurnToAngleP.finalslope = PI / 2;
//          state1 = 0;
//        }
		if(0){printf("\n in if(0)\n");}
        else
        {
			
          state1 = 1;
          //printf("Ball x:%d y:%d v_x:%f v_y:%f\n", state->ballPos.x, state->ballPos.y, state->ballVel.x, state->ballVel.y);
          sID = SkillSet::DWGoToPoint;
          sParam.DWGoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3.5) /*/4*/;
          int temp = getBotDestPointY();
          sParam.DWGoToPointP.y = temp;
					printf("\n Predicting point = %d \n",temp);
          sParam.DWGoToPointP.finalSlope = -PI / 2;
          
          // Util::Logger::toStdOut("%d  Bot showing goalkeeping skills.\n",botID);
          //  Util::Logger::toStdOut("GO to Point %d, %d\n",ForwardX(-(HALF_FIELD_MAXX) + DBOX_WIDTH),temp);
          // Util::Logger::toStdOut("Boll Pos.%d, %d\n",state->ballPos.x,state->ballPos.y);
          //  tState = COMPLETED;
        }
      }
      skillSet->executeSkill(sID, sParam);
    }

    bool isGoalKeeperInPosition()
    {
      //Util::Logger::toStdOut("Half_field_max : %d, Dbox_width : %d\n",HALF_FIELD_MAXX,DBOX_WIDTH);
      if ((ForwardX(state->homePos[botID].x) >  (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3)) &&
          (ForwardX(state->homePos[botID].x) <= (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*5)) &&
          (state->homePos[botID].y >= OUR_GOAL_MINY - DBOX_WIDTH) &&
          (state->homePos[botID].y <= (OUR_GOAL_MAXY + DBOX_WIDTH)))
        return true;
      else
        return false;

    }
    
    int getBotDestPointY()
    {
      Vector2D<int> ballFinalpos, botDestination, point;
      int flag=2;
     // balldist = state->ballPos.x;
	//		if(state->ballPos.x>0)
		//		balldist =  
				
      float balldistratio = fabs(state->ballPos.x)/(1*(HALF_FIELD_MAXX-DBOX_WIDTH-6*BOT_RADIUS))<1 ? fabs(state->ballPos.x)/(HALF_FIELD_MAXX-DBOX_WIDTH-6*BOT_RADIUS):1 ;
      
      point.y = balldistratio*SGN(state->ballPos.y)*MIN(fabs(state->ballPos.y), OUR_GOAL_MAXY); 
        printf("\npoint.y = %d\n",point.y);
      /* Workaround for ball velocity 0*/
      if( ( ( fabs(state->ballVel.y) + fabs(state->ballVel.x) < 50) ) || (ForwardX(state->ballVel.x)<0 && ForwardX(state->ballVel.x)>(-50)) )
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
        point.y = (state->ballVel.y/state->ballVel.x)*(ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*4) - (state->ballPos.x)) + state->ballPos.y,flag = 1;
      }
      if(point.y > BOT_RADIUS) point.y -= 1.5*BOT_RADIUS;
      else if(point.y < -BOT_RADIUS) point.y += 1.5*BOT_RADIUS;
         //point.y = point.y + (state->ballVel.y > 0 ? BOT_RADIUS : -BOT_RADIUS);
        
      /* Set Limits on y to not exceed DBOX Y Limits*/
      if(point.y < OUR_GOAL_MINY + 2*BOT_RADIUS)
       {
         if(point.y >-HALF_FIELD_MAXY)
         point.y = OUR_GOAL_MINY + 2*BOT_RADIUS;
        else
          point.y = 0;
       }
       
      else if(point.y > OUR_GOAL_MAXY - 2*BOT_RADIUS)
      {
         if(point.y < HALF_FIELD_MAXY)
          point.y = OUR_GOAL_MAXY - 2*BOT_RADIUS;
          else
          point.y = 0;
       }
//      if(flag==0)
//      printf("\n Center ");
//      else if(flag==1)
//        printf("\n Predicting");
//        else 
//          printf("\n Follow Ball");
          
      return point.y;
    }
}; // class TDefense
} // namespace Strategy

#endif // TTCharge_HPP