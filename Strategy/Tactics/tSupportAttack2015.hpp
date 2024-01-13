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
  class TSupportAttack2015 : public Tactic
  {
    static const int offset = 400;
    // Corner Case: wall, ball, bot in line
    float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
	
    Point2D<int> prevBotPos;
    float prevBotAngle;
  public:
    TSupportAttack2015(const BeliefState* state, int botID) :
      Tactic(Tactic::SupportAttack2015, state, botID)
    {
      iState = ATTACK_LINGO;
      for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      hasAchievedOffset = 0;
    } // TAttack

    ~TSupportAttack2015()
    { } // ~TAttack
    enum InternalState
    {
		ATTACK_LINGO,
		DEFEND,
//		FORM_SWARM,
		ATTACKING,
 /*     APPROACHING,
      SPINNING_CCW = -1,
      SPINNING_CW = 1,
      ATTACKING*/
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

			float dist = Vector2D<int>::dist(state->homePos[botID], state->homePos[botID]);
			int ourBot = state->ourBotNearestToBall;
			//int oppBot = state->oppBotNearestToBall;

			if(abs(state->homePos[botID].y) < OUR_GOAL_MAXY - BOT_RADIUS && ForwardX(state->homePos[botID].x) < -HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_HEIGHT)
			{
				
				sID = SkillSet::GoToPoint;
				
				
				sParam.GoToPointP.align = true;
				sParam.GoToPointP.increaseSpeed = true;
				if(dist < 2*BOT_BALL_THRESH)
				{
					sParam.GoToPointP.x = state->homePos[botID].x;
					sParam.GoToPointP.y = state->homePos[botID].y - 2*SGN(state->ballPos.y)*BOT_RADIUS;
				}
				else
				{
					sParam.GoToPointP.x = 0;
					sParam.GoToPointP.y = state->homePos[botID].y + SGN(state->homePos[botID].y - state->homePos[0].y)*dist;
				}
				skillSet->executeSkill(sID, sParam);
				return;
			}


			switch(iState)
			{
				case ATTACK_LINGO :
				{
					/*if(state->ballPos.x < 0 && state->ballVel.x < 0 && state->ballPos.x > state->homePos[botID].x && 
							state->ballPos.x > ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH)*0.8)
					{
						iState = DEFEND;
						break;
					}*/
					
					float factorX = 0.00005;
					float factorY = 0.00005;
//					if(state->ballVel.x > 10) factorX = 0.001;
//					if(((state->ballVel.y > 10 && state->ballPos.y < OPP_GOAL_MINY) || (state->ballVel.y < -10 && state->ballPos.y > OPP_GOAL_MAXY)) && state->ballPos.x > HALF_FIELD_MAXX/3)
//						factorY = 0.001;

					
					float ang = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
					int targetX = state->ballPos.x + dist*factorX*state->ballVel.x - BOT_RADIUS*cos(ang);
					int targetY = state->ballPos.y + dist*factorY*state->ballVel.y - BOT_RADIUS*sin(ang);


					sID = SkillSet::GoToPoint;
					
					if(ourBot != botID && ForwardX(state->ballPos.x) < 2*(HALF_FIELD_MAXX - GOAL_DEPTH)/3 && 
						abs(state->ballPos.y) > HALF_FIELD_MAXY/3 && Vector2D<int>::dist(state->ballPos, state->homePos[ourBot]) < 2*BOT_BALL_THRESH)
					{
						targetY = state->ballPos.y - SGN(state->ballPos.y)*5*BOT_RADIUS;
						sParam.GoToPointP.x = targetX - BOT_RADIUS;
						sParam.GoToPointP.y = targetY;
						sParam.GoToPointP.align = true;
						sParam.GoToPointP.increaseSpeed = true;
					
						skillSet->executeSkill(sID, sParam);
						return;
					}
					else if(ForwardX(targetX) < (-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS) && abs(state->ballPos.y) < HALF_FIELD_MAXY*0.6)
					{
						targetX = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS);
					}
					else if((ForwardX(targetX )> (HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS)) && (abs(targetY) < OUR_GOAL_MAXY + BOT_RADIUS))
					{
						targetY = SGN(targetY)*(OUR_GOAL_MAXY + BOT_RADIUS) ;
						targetX = ForwardX(HALF_FIELD_MAXX - GOAL_DEPTH - 1*BOT_RADIUS) ;
					}
					
					if(ForwardX(state->ballPos.x )> HALF_FIELD_MAXX - GOAL_DEPTH - BOT_RADIUS*2 &&
						ForwardX(state->homePos[botID].x) > HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS && 
							(
							(state->homePos[botID].y > state->ballPos.y && state->ballPos.y < 0) || 
							(state->homePos[botID].y < state->ballPos.y && state->ballPos.y > 0)
							)
						)
					{
						targetX = ForwardX(HALF_FIELD_MAXX - GOAL_DEPTH - DBOX_WIDTH - 4*BOT_RADIUS);
						if (state->homePos[botID].y > state->ballPos.y && state->ballPos.y < 0)
						{
							targetY = OPP_GOAL_MINY - BOT_RADIUS;
						}
						else if(state->homePos[botID].y < state->ballPos.y && state->ballPos.y > 0)
						{
							targetY = OPP_GOAL_MAXY + BOT_RADIUS;
						}
					}
					sParam.GoToPointP.x = targetX - BOT_RADIUS;
					sParam.GoToPointP.y = targetY;
					sParam.GoToPointP.align = true;
					sParam.GoToPointP.increaseSpeed = true;
					
					if(ourBot != botID)
					{
						if(Vector2D<int>::dist(state->ballPos, state->homePos[ourBot]) < dist && state->ballPos.x > 0)
						{
							sParam.GoToPointP.y = state->ballPos.y - SGN(state->homePos[ourBot].y - state->ballPos.y)*OPP_GOAL_MAXY;
						}
					}


					skillSet->executeSkill(sID, sParam);
					break;
				}
				
				
				case DEFEND :
				{
					if(dist < 2*BOT_BALL_THRESH && ForwardX(state->ballPos.x)> ForwardX(state->homePos[botID].x))
					{
						sID = SkillSet::GoToPoint;
						sParam.GoToPointP.x = state->ballPos.x;
						sParam.GoToPointP.y = state->ballPos.y;
						sParam.GoToPointP.finalslope = (state->ballVel.y/state->ballVel.x);
						skillSet->executeSkill(sID, sParam);
						return;
					}
					if (ForwardX(state->ballPos.x) < 0 && state->ballVel.x < 0 && ForwardX(state->ballPos.x )>ForwardX( state->homePos[botID].x) && ForwardX(state->ballPos.x) > (-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS))
					{
						if( ourBot != botID && 
							ForwardX(state-> ballPos.x)> ForwardX(state->homePos[ourBot].x) && 
							dist < 2*BOT_RADIUS &&
							Vector2D<int>::dist(state->homePos[botID], state->homePos[ourBot]) > 4*BOT_RADIUS)
						{
							int defendX = ForwardX((-HALF_FIELD_MAXX + GOAL_DEPTH)*0.8);
							sID = SkillSet::DefendPoint;
							if( abs(state->ballPos.x - defendX) < 5*BOT_RADIUS)
								sParam.DefendPointP.x = defendX;
							else if( ForwardX(state->ballPos.x) < 10*BOT_RADIUS)
								sParam.DefendPointP.x = defendX + 4*BOT_RADIUS;
							else if( state->ballPos.x < 15*BOT_RADIUS)
								sParam.DefendPointP.x = defendX + 9*BOT_RADIUS;
							sParam.DefendPointP.y = 0;
							sParam.DefendPointP.finalslope = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
							skillSet->executeSkill(sID, sParam);
						}
						else
						{
							iState = ATTACKING;
							break;
						}
					}
					else
					{
						iState = ATTACK_LINGO;
						break;
					}
				}
				case ATTACKING :
				{
					if(Vector2D<int>::dist(state->ballPos, state->homePos[ourBot]) < 2*BOT_BALL_THRESH && 
						abs(state->ballPos.y) > HALF_FIELD_MAXY - 2*BOT_RADIUS &&		//change
						abs(state->homePos[ourBot].y) > HALF_FIELD_MAXY - 2*BOT_RADIUS && 
						ForwardX(state->homePos[botID].x) > ForwardX(state->homePos[ourBot].x))
					{
						iState = ATTACK_LINGO;
						break;
					}
					else if(state->ballPos.x < 0 && state->ballVel.x < 0   && ForwardX(state->ballPos.x )> ForwardX(state->homePos[botID].x) && ForwardX(state->ballPos.x )>(-HALF_FIELD_MAXX + GOAL_DEPTH)*0.8)
					{
						iState = DEFEND;
						break;
					}
					
					if(state->homePos[botID].x < state->ballPos.x && state->ballPos.x > ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH)*0.8)
					{
						sID = SkillSet::GoToPoint;
						sParam.GoToPointP.x = state->ballPos.x;
						sParam.GoToPointP.y = state->ballPos.y;
						sParam.GoToPointP.finalslope = (state->ballVel.y/state->ballVel.x);
						skillSet->executeSkill(sID, sParam);
						return;
					}
					else
					{
						iState = ATTACK_LINGO;
						break;
					}
				}
			}

	}
  };
}