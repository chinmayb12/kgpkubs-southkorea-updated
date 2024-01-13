#ifndef TTATTACK_HPP
#define TTATTACK_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "intersection.hpp"
#define ANGLE_TO_DIST 0
namespace Strategy
{
  class TAttack : public Tactic
  {
    static const int offset = 400;
    // Corner Case: wall, ball, bot in line
    float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
  public:
    TAttack(const BeliefState* state, int botID) :
      Tactic(Tactic::Attack, state, botID)
    {
      iState = APPROACHING;
      for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      hasAchievedOffset = 0;
    } // TAttack

    ~TAttack()
    { } // ~TAttack
    enum InternalState
    {
      APPROACHING,
      SPINNING_CCW = -1,
      SPINNING_CW = 1,
      ATTACKING
    } iState;
    int hasAchievedOffset;
    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
	  float angle_difference = firaNormalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)- state->homeAngle[*(freeBots.begin())]);
      int minwt = Vector2D<int>::dist(state->homePos[*(freeBots.begin())],state->ballPos) + angle_difference * ANGLE_TO_DIST;
	  
	  for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = Vector2D<int>::dist(state->homePos[*it],state->ballPos);
        float botballangle = normalizeAngle(Vector2D<int>::angle(state->ballPos, state->homePos[*it]));
        //TODO might require normalization
        float botball_orientation_diff = MIN(fabs(botballangle-state->homeAngle[*it]),fabs(botballangle-(state->homeAngle[*it]+PI)));
        float finalOrientationDiff = normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)-botballangle);
//		angle_difference =  fabs(firaNormalizeAngle(state->homeAngle[*it]-normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos))))+ fabs(firaNormalizeAngle((Vector2D<int>::angle(state->homePos[*it],Vector2D<int>(OPP_GOAL_X, 0)))));
      angle_difference = botball_orientation_diff + finalOrientationDiff;
  //float x_diff = ForwardX(state->ballPos.x)-ForwardX(state->homePos.x);
				float weight;
    //printf("%d >>>>>>>>>> %f , %f\n", *it,dis_from_ball,angle_difference);
				weight = dis_from_ball + ANGLE_TO_DIST * angle_difference;
				if(*it == prevID)
					weight -= HYSTERESIS;
        if(weight < minwt)
        {
          minwt = dis_from_ball ;
          minv = *it;
        }
      }
      //Util::Logger::toStdOut("Selected bot %d\n", minv);
	  printf(" :: %d ::",minv);
      //assert(tParam=0);
      return minv;

    } // chooseBestBot

    bool pointxInField(Vector2D<int> final)
    {
      if((final.x < HALF_FIELD_MAXX - (BALL_AT_CORNER_THRESH) && final.x > -HALF_FIELD_MAXX + (BALL_AT_CORNER_THRESH)))
      {
        if((final.y < HALF_FIELD_MAXY - BALL_AT_CORNER_THRESH && final.y > -HALF_FIELD_MAXY + BALL_AT_CORNER_THRESH))
        {
          return true;
        }
        else return false;
      }
      else  return false;
    }
    void execute(const Param& tParam)
    {
      printf("Attack BotID: %d\n",botID);
      static Vector2D<float> lastVel[10];
			static int index = 0;
			if(index < 10) {
				lastVel[index].x = state->ballVel.x;
				lastVel[index].y = state->ballVel.y;
				index = (index + 1) % 10;
			}
			Vector2D<float> avgBallVel(0.0,0.0);
			for(int i=0;i<10;i++) {
				avgBallVel.x += lastVel[i].x;
				avgBallVel.y += lastVel[i].y;
			}
			avgBallVel.x /= 10.0;
			avgBallVel.y /= 10.0;
			
      float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
      movementError[movementErrorIndex++] = (Vector2D<int>::distSq(prevBotPos, state->homePos[botID])) + (prevBotAngle - state->homeAngle[botID])*(prevBotAngle - state->homeAngle[botID])*50000;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      movementErrorIndex %= 10;
      movementErrorSum = 0;
      for(int i=0; i<10; i++)
        movementErrorSum += movementError[i];
			//printf("movement error = %f, isRotate = %d\n", movementErrorSum, tParam.AttackP.rotateOnError);
      if(movementErrorSum < 500 && tParam.AttackP.rotateOnError)
      {
        sID = SkillSet::Spin;
        sParam.SpinP.radPerSec = MAX_BOT_OMEGA * (state->homePos[botID].y > 0? ForwardX(1): ForwardX(-1));
        skillSet->executeSkill(sID, sParam);
        return;
      }

      
        if(iState == SPINNING_CW || iState == SPINNING_CCW)
        {
          if(dist > BOT_BALL_THRESH * 1.1)
          {
            iState = APPROACHING;
          }
          else
          {
            sID = SkillSet::Spin;
            if(iState == SPINNING_CW)
              sParam.SpinP.radPerSec = (-MAX_BOT_OMEGA);
            else
              sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
            skillSet->executeSkill(sID, sParam);
            return;
          }
        }
        if(dist < BOT_BALL_THRESH)
        {
          /*Ball with bot, but 90 deg angle. So spin*/
          if(fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 10 && fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 10)
          {
            if(state->ballPos.y > 0)
              iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;
            else
              iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;
          }
          else
          {
            /* Ball is with bot. So go to goal */
            printf("\n Going to Ball \n ");
		    sID = SkillSet::GoToPoint;
            sParam.GoToPointP.align = false;
            sParam.GoToPointP.x = OPP_GOAL_X;
            sParam.GoToPointP.y = 0;
            sParam.GoToPointP.finalslope = Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos);
            sParam.GoToPointP.increaseSpeed = 1;
            skillSet->executeSkill(sID, sParam);
            tState = RUNNING;
            iState = ATTACKING;
          }
        }
        else
        {          
          /* Ball is not with bot so go to ball first */
          sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = true;
          float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
          float factor = (int)Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
          factor /= 5*MAX_BOT_SPEED;
          factor =0;
          int ballPosX = state->ballPos.x;// + factor * state->ballVel.x;
          int ballPosY = state->ballPos.y;// + factor * state->ballVel.y;
          
          float offset = TAttack::offset;//TAttack::offset * state->ballVel.abs()/6000.0+
		  if(ForwardX(state->homePos[botID].x) < ballPosX)
				offset = 0;
		  int x3 = (ballPosX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
          int y3 = (ballPosY * (ballgoaldist + offset)) / ballgoaldist;
          /// logarithmic search to place offset point in field. */
        //float offset = 600;
					/****************************** added velocity factor in offset *****************/
			factor = 0.4;
			int targetX=0,targetY=0;					
			{
				//printf("Ball Veclocity : x = %f  y = %f\n",avgBallVel.x,avgBallVel.y);
				int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
			//	printf("delta x = %d   delta y = %d\n",(int)(factor * state->ballVel.x),(int)(factor * state->ballVel.y));
				targetX = state->ballPos.x + (int)(factor * avgBallVel.x);
				targetY = state->ballPos.y + (int)(factor * avgBallVel.y);
			}
					/**************************** done velocity factor ****************************/
					    
					/*************************** Weighted offset *********************************/
					//weighted value of previously calculated offset and one using ball velocity
					// P = (P1 + W*P2)/(W + 1)
				float weight = ((ForwardX(avgBallVel.x) < -100) ? -ForwardX(avgBallVel.x)-100 : 0.00)*sqrt(avgBallVel.y*avgBallVel.y) / (200.0);
				Vector2D<int> final(0,0);
				final.x = x3 + weight * targetX;			
				final.y = y3 + weight * targetY;
				final.x = final.x/(weight + 1);
				final.y = final.y/(weight + 1);
				x3 = final.x, y3 = final.y;
					/************************ Done offset ***************************************/
					
				SkillSet::comm->addCircle(x3,y3,300);
					
          while(!LocalAvoidance::isPointInField(Point2D<int>(x3, y3))) 
          {
            if(!LocalAvoidance::isPointInField(state->ballPos))
            {
              offset= 0;
              x3 = (ballPosX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
              y3 = (ballPosY * (ballgoaldist + offset)) / ballgoaldist;
              break;
            }
            offset /= 1.25;
            if(offset <= 1.0)
              break;
            x3 = (ballPosX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
            y3 = (ballPosY * (ballgoaldist + offset)) / ballgoaldist;
          }
          offset/=1.25;
          /// log search to place offset at a point not co-inciding with a bot.
          while(1)
          {
            bool flag = false;
            for(int i=0; i < HomeTeam::SIZE; i++)
            {
              if(Intersection::pointInCircle(x3, y3, state->homePos[i].x, state->homePos[i].y, BOT_RADIUS*3))
              {
                if(i == botID)
                  continue;
                flag = true;
                offset /= 1.1;
              }
            }
            for(int i=0; i < AwayTeam::SIZE; i++)
            {
              if(Intersection::pointInCircle(x3, y3, state->awayPos[i].x, state->awayPos[i].y, BOT_RADIUS*3))
              {
                flag = true;
                offset /= 1.1;
              }
            }
            //x3 = (ballPosX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
            //y3 = (ballPosY * (ballgoaldist + offset)) / ballgoaldist;
            if(!flag)
              break;
            if(offset <= 2.0)
            {
              offset = 0;
              //x3 = (ballPosX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
              //y3 = (ballPosY * (ballgoaldist + offset)) / ballgoaldist;
              break;
            }
          }
          Vector2D<int> offsetpt(x3, y3);
          int dist2 = Vector2D<int>::dist(offsetpt, state->homePos[botID]);
          if(dist2 < 200)
            hasAchievedOffset = 1;
          else if(dist > 2 * offset)
            hasAchievedOffset = 0;

          if(ForwardX(state->ballPos.x ) < ForwardX(state->homePos[botID].x)) 
            hasAchievedOffset = 0;
            
          sParam.GoToPointP.x = x3;
          sParam.GoToPointP.y = y3;
          sParam.GoToPointP.finalslope = Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
          
          sParam.GoToPointP.increaseSpeed = 0;
          if(hasAchievedOffset)
          {

            sParam.GoToPointP.x = state->ballPos.x ;
            sParam.GoToPointP.y = state->ballPos.y;
            sParam.GoToPointP.finalslope = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
            sParam.GoToPointP.increaseSpeed = 1;

          }
		    cout<<" BallPos :: ( "<<state->ballPos.x<<" , "<<state->ballPos.y<<" ) :: "<<"Destination : "<<sParam.GoToPointP.x<<" "<<sParam.GoToPointP.y<<endl ;
          skillSet->executeSkill(sID, sParam);
        }
      
    }
  }; // class TAttack
} // namespace Strategy

#endif // TTCharge_HPP