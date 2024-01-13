#ifndef TDWDEFENDER_HPP
#define TDWDEFENDER_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "pose.h"

namespace Strategy
{
    class TDWDefender : public Tactic
    {
	public:
		static const int offset = 400;
		  int hasAchievedOffset;
		
			TDWDefender(const BeliefState* state, int botID) :
			  Tactic(Tactic::DWDefender, state, botID)
			{
			  iState = DEFENDING;
			} // TDWDefender	
			~TDWDefender()
			{ } // ~TDWDefender

		enum InternalState
		{
		  DEFENDING,
		  SPINNING_CCW = -1,
		  SPINNING_CW = 1,
		  ATTACKING,
		} iState;
		inline bool isActiveTactic(void) const
		{
			return false;
		}
		   //CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
    //Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

		inline bool isPointinField(const Vector2D<int>& point){
			if(abs(point.x) <= HALF_FIELD_MAXX && abs(point.y) <= HALF_FIELD_MAXY)
				return 1;
			else 
				return 0;
		}
		int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
		{
		  assert(tParam != 0);
		  int minv   = *(freeBots.begin());
		  int mindis = 1000000000;
		  Vector2D<int> tGoToPoint(tParam->PositionP.x, tParam->PositionP.y);
		  
		  for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
		  {
			// TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
			float dis_from_point = (state->homePos[*it] - tGoToPoint).absSq();
			if(dis_from_point < mindis)
			{
			  mindis = dis_from_point;
			  minv = *it;
			}
		  }
		  printf("%d assigned Position\n", minv);
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

    Pose executeattack()
    {
        
		Vector2D<int> ballPos(state->ballPos.x,state->ballPos.y);
        Vector2D<int> homePos(state->homePos[botID].x, state->homePos[botID].y);
        Vector2D<int> opp_goal(HALF_FIELD_MAXX, 0);
        static Vector2D<float> lastVel[10];
                    static int index = 0;
                    if(index < 10)
                    {
                        lastVel[index].x = state->ballVel.x;
                        lastVel[index].y = state->ballVel.y;
                        index = (index + 1) % 10;
                    }
                    Vector2D<float> avgBallVel(0.0,0.0);
                    for(int i=0;i<10;i++)
                    {
                        avgBallVel.x += lastVel[i].x;
                        avgBallVel.y += lastVel[i].y;
                    }
                    avgBallVel.x /= 10.0;
                    avgBallVel.y /= 10.0;


                    float dist = Vector2D<int>::dist(ballPos, homePos);
          /* Ball is not with bot so go to ball first */
          //sParam.GoToPointP.align = true;
          float ballgoaldist = Vector2D<int>::dist(ballPos, opp_goal);
          float factor = (int)Vector2D<int>::dist(ballPos,homePos);
          factor /= 5*MAX_BOT_SPEED;
          factor =0;
          //int ballPosX = state->ballPos.x;// + factor * state->ballVel.x;
          //int ballPosY = state->ballPos.y;// + factor * state->ballVel.y;
          
          float offset = offset;//TAttack::offset * state->ballVel.abs()/6000.0+
		  if(homePos.x < ballPos.x)
				offset = 0;
		  int x3 = (ballPos.x * (ballgoaldist + offset) - offset * HALF_FIELD_MAXX) / ballgoaldist;
          int y3 = (ballPos.y * (ballgoaldist + offset)) / ballgoaldist;
          /// logarithmic search to place offset point in field. */
        //float offset = 600;
          
          
          
          /****************************** added velocity factor in offset *****************/
         factor = 0.4;
         int targetX=0,targetY=0;         
          {
            printf("Ball Veclocity : x = %f  y = %f\n",avgBallVel.x,avgBallVel.y);
                        int ballBotDist = (int)Vector2D<int>::dist(homePos,ballPos);
                    //    printf("delta x = %d   delta y = %d\n",(int)(factor * state->ballVx),(int)(factor * state->ballVy));
            
                        targetX = ballPos.x + (int)(factor * avgBallVel.x);
                        targetY = ballPos.y + (int)(factor * avgBallVel.y);
        
          }

          float weight = (avgBallVel.x < -100 ? -(avgBallVel.x)-100 : 0.00)*sqrt(avgBallVel.y*avgBallVel.y) / (200.0);
          Vector2D<int> final(0,0);
          final.x = x3 + weight * targetX;      
          final.y = y3 + weight * targetY;
          final.x = final.x/(weight + 1);
          final.y = final.y/(weight + 1);
          
          x3 = final.x, y3 = final.y;
          /************************ Done offset ***************************************/
          
                    //SkillSet::comm->addCircle(x3,y3,300);

          while(isPointinField(Vector2D<int>(x3, y3)))
          {
            if(isPointinField(ballPos))
            {
              offset= 0;
              x3 = (ballPos.x * (ballgoaldist + offset) - offset * HALF_FIELD_MAXX) / ballgoaldist;
              y3 = (ballPos.y * (ballgoaldist + offset)) / ballgoaldist;
              break;
            }
            offset /= 1.25;
            if(offset <= 1.0)
              break;
            x3 = (ballPos.x * (ballgoaldist + offset) - offset * HALF_FIELD_MAXX) / ballgoaldist;
            y3 = (ballPos.y * (ballgoaldist + offset)) / ballgoaldist;
          }
          offset/=1.25;
          /// log search to place offset at a point not co-inciding with a bot.
          Point2D<int> offsetpt(x3,y3);
          int dist2 = Vector2D<int>::dist(offsetpt, homePos);
          if(dist2 < 200)
            hasAchievedOffset = 1;
          else if(dist > 2 * offset)
            hasAchievedOffset = 0;

          if(ballPos.x  < homePos.x)
            hasAchievedOffset = 0;

          Pose attackp(x3,y3,Vector2D<int>::angle(opp_goal,ballPos));

          if(hasAchievedOffset)
          {

            return Pose(ballPos.x,ballPos.y,Vector2D<int>::angle(ballPos, homePos));
          }
          else
          {
              return attackp;
          }
    }

    void execute(const Param& tParam)
    {
		sID = SkillSet::DWGoToPoint;
        Vector2D<int> ballPos(state->ballPos.x,state->ballPos.y);
        Vector2D<int> homePos(state->homePos[botID].x, state->homePos[botID].y);
        Vector2D<int> opp_goal(HALF_FIELD_MAXX, 0);
		Pose finalPose;

        ////needs to be corrected opp_pos
        //Vector2D<int> opp_pos(state->awayX[state->oppBotNearestToBall],state->awayY[state->oppBotNearestToBall]);
        float ang1,temp1;
        float dist2=100000000;
        int i;
        for(i=0;i<5;i++)
        {
            Vector2D <int> awayPos(state->awayPos[i].x, state->awayPos[i].y);
          temp1=Vector2D<int>::dist(ballPos,awayPos);
          if(dist2>temp1)dist2=temp1;

        }

        float dist = Vector2D<int>::dist(ballPos, homePos);
        //float dist2= Vector2D<int>::dist(ballPos,opp_pos);

        /////start changes from here //////
        if(ballPos.x>0)
          {
            if(ballPos.x>HALF_FIELD_MAXX-DBOX_WIDTH)
            {
					iState=DEFENDING;
              		sParam.DWGoToPointP.x = (-0.4*(HALF_FIELD_MAXX-DBOX_WIDTH));
					sParam.DWGoToPointP.y = ballPos.y;
					sParam.DWGoToPointP.finalSlope = Vector2D<int>::angle(opp_goal, ballPos);
					sParam.DWGoToPointP.finalVelocity = 0;
					skillSet->executeSkill(sID, sParam);
					return;
            }
            else if( abs(state->ballVel.x) > 50 && state->ballVel.x<0 )
            {
				iState=DEFENDING;
                  ang1 = atan(state->ballVel.y/state->ballVel.x);
                  temp1=ballPos.y -((ballPos.x)-(-HALF_FIELD_MAXX + DBOX_WIDTH))*tan(ang1);
                  if(temp1>HALF_FIELD_MAXY)
					  temp1 =  HALF_FIELD_MAXY;
                  if(temp1<-HALF_FIELD_MAXY)
					  temp1=-HALF_FIELD_MAXY;
				  
				  
                  sParam.DWGoToPointP.x = -0.7*(HALF_FIELD_MAXX - DBOX_WIDTH);
				  sParam.DWGoToPointP.y = temp1;
				  sParam.DWGoToPointP.finalSlope = 0;
				  sParam.DWGoToPointP.finalVelocity = 0;
				  skillSet->executeSkill(sID, sParam);
					return;
            }
            else
            {
				iState=DEFENDING;
			   sParam.DWGoToPointP.x = (0.3*(ballPos.x +(HALF_FIELD_MAXX-DBOX_WIDTH ))-(HALF_FIELD_MAXX-DBOX_WIDTH));
			   sParam.DWGoToPointP.y = ballPos.y;
			   sParam.DWGoToPointP.finalSlope = 0;
			   sParam.DWGoToPointP.finalVelocity = 0;
			   skillSet->executeSkill(sID, sParam);
					return;
            }

          }
          else
          {

             if(abs (dist-dist2)<2*BOT_BALL_THRESH &&  ballPos.x > -HALF_FIELD_MAXX+1.5*GOAL_DEPTH )
             {
                 ///ATTACK FUNCTION
				 iState=ATTACKING;
                finalPose =  executeattack();
				sParam.DWGoToPointP.x = finalPose.x();
				sParam.DWGoToPointP.y = finalPose.y();
				sParam.DWGoToPointP.finalSlope = finalPose.theta();
				sParam.DWGoToPointP.finalVelocity = 0;
				skillSet->executeSkill(sID, sParam);
					return;
                //return Pose(ballPos.x,ballPos.y,0);

             }
            // else return  Pose(HALF_FIELD_MAXX,ballPos.y,0);

             if (ballPos.x < homePos.x &&  ballPos.x >-HALF_FIELD_MAXX+1.5*GOAL_DEPTH )
             {
              ////ATTACK THE BALL
				iState=ATTACKING;
               finalPose = executeattack();
				sParam.DWGoToPointP.x = finalPose.x();
				sParam.DWGoToPointP.y = finalPose.y();
				sParam.DWGoToPointP.finalSlope = finalPose.theta();
				sParam.DWGoToPointP.finalVelocity = 0;	
				skillSet->executeSkill(sID, sParam);
					return;
                //return Pose(ballPos.x,ballPos.y,0);

             }
             if(ballPos.x > homePos.x &&  ballPos.x >-HALF_FIELD_MAXX+1.5*GOAL_DEPTH )
             {
                   if(state->ballVel.x<50)
                   {
                       //Working properly
					   iState=ATTACKING;
						sParam.DWGoToPointP.x = ballPos.x;
						sParam.DWGoToPointP.y = ballPos.y;
						sParam.DWGoToPointP.finalSlope = 0;
						sParam.DWGoToPointP.finalVelocity = 0;
						skillSet->executeSkill(sID, sParam);
							return;
                   }

                    if(ballPos.x > -0.7*(HALF_FIELD_MAXX-DBOX_WIDTH))
                      {
                        if(state->ballVel.x > 100)
						{
							iState=DEFENDING;
							ang1 = atan(state->ballVel.y/state->ballVel.x);
                            temp1=ballPos.y -((ballPos.x)-(-HALF_FIELD_MAXX + DBOX_WIDTH))*tan(ang1);
                            if(temp1>HALF_FIELD_MAXY || temp1 < -HALF_FIELD_MAXY)
                            temp1 =  ballPos.y;
                        }
                        else{
							iState=DEFENDING;
                            temp1 = ballPos.y;
                        }
                             ///CAN BE CHANGED TO HALFFIELDMAXX
							sParam.DWGoToPointP.x = -0.7*(HALF_FIELD_MAXX-DBOX_WIDTH);
							sParam.DWGoToPointP.y = temp1;
							sParam.DWGoToPointP.finalSlope = 0;
							sParam.DWGoToPointP.finalVelocity = 0;
							skillSet->executeSkill(sID, sParam);
								return;
                        }
                        else
                          {
								iState=ATTACKING;
								finalPose = executeattack();
								sParam.DWGoToPointP.x = finalPose.x();
								sParam.DWGoToPointP.y = finalPose.y();
								sParam.DWGoToPointP.finalSlope = finalPose.theta();
								sParam.DWGoToPointP.finalVelocity = 0;	
								skillSet->executeSkill(sID, sParam);
									return;
                          }
                }
             if(ballPos.x <-HALF_FIELD_MAXX+1.5*GOAL_DEPTH && abs(ballPos.y)>OUR_GOAL_MAXY + 1.8*BOT_RADIUS)
             {
					iState=ATTACKING;
					finalPose = executeattack();
					sParam.DWGoToPointP.x = finalPose.x();
					sParam.DWGoToPointP.y = finalPose.y();
					sParam.DWGoToPointP.finalSlope = finalPose.theta();
					sParam.DWGoToPointP.finalVelocity = 0;	
					skillSet->executeSkill(sID, sParam);
						return;
              ///ATTACK  THE BALL
             }
          }
			
			sParam.DWGoToPointP.x = ballPos.x;
			sParam.DWGoToPointP.y = ballPos.y;
			sParam.DWGoToPointP.finalSlope = 0;
			sParam.DWGoToPointP.finalVelocity = 0;
			skillSet->executeSkill(sID, sParam);
			
			if(HALF_FIELD_MAXY-abs(homePos.y) < BOT_RADIUS  && Vector2D<int>::dist(ballPos,homePos) < BOT_BALL_THRESH)
			{
				if(fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 10 && fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 10)
				{
					sID = SkillSet::Spin;
					if(state->ballPos.y > 0)
					{
						sParam.SpinP.radPerSec = (-MAX_BOT_OMEGA);
						iState = SPINNING_CW;
					}
						
					else
					{
						sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
						iState =SPINNING_CCW;
					}
						
				}

			}
				
		return;

		}
	}; 
	}
#endif //TTDWDefender