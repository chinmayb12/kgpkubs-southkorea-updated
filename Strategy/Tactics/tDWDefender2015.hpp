#ifndef TDWDEFENDER2015_HPP
#define TDWDEFENDER2015_HPP

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
    class TDWDefender2015 : public Tactic
    {
		
	public:
		  int hasAchievedOffset;
		
			TDWDefender2015(const BeliefState* state, int botID) :
			  Tactic(Tactic::DWDefender2015, state, botID)
			{
			  iState = POSITIONING;
			} // TDWDefender	
			~TDWDefender2015()
			{ } // ~TDWDefender

		enum InternalState
		{ 
		  POSITIONING,
		  INTERCEPTING,
		  DEFENDING,
		  SPINNING_CCW ,
		  SPINNING_CW ,
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
			int minv1   =-1,minv2  = *(freeBots.begin()) , dist;
			int mindis1 = INT_MAX,mindis2 = INT_MAX;
			if(ForwardX(state->ballPos.x) > -HALF_FIELD_MAXX/2)
			{
				for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
				{
					dist = abs(state->homePos[*it].x + HALF_FIELD_MAXX/2);
					if(ForwardX(state->homePos[*it].x) < -HALF_FIELD_MAXX/2 && dist < mindis1 )
					{
						minv1 = *it;
						mindis1 = dist;
					}
					else if( dist < mindis2)
					{
						minv2 = *it;
						mindis2 = dist;
					}
				}
			}
			else
			{
				for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
				{
					dist = Vector2D<int>::dist(state->homePos[*it],state->ballPos);
					if(dist < mindis2)
					{
						mindis2 = dist;
						minv2 = *it;
					}
				}
			}
			cout<<"Defender :: "<<minv2<<" or "<<minv1<<endl;
			if(minv1 ==-1)
				return minv2;
			else
				return minv1;
		}// chooseBestBot
	
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
	static bool isFirstRun=true;
		cout<<"Defender :: "<<botID<<endl ;
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
			//______________________________________
			
			
	float ang1 = atan(state->ballVel.y/state->ballVel.x);
	float distBotBall = Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
	float distOppBall = Vector2D<int>::dist(state->ballPos,state->awayPos[state->oppBotNearestToBall]);
	int oppthreshX = ForwardX(HALF_FIELD_MAXX/2 - 2*BOT_RADIUS) ;
	int ourthreshX = ForwardX(-HALF_FIELD_MAXX/4 - 2*BOT_RADIUS) ;
	switch(iState)
	{
	case POSITIONING :        // if ball is in opponent side 
	{  
	   if(ForwardX(state->ballPos.x) < ForwardX(oppthreshX) && ForwardX(state->ballPos.x ) > ForwardX(ourthreshX))
	   {
		   iState = INTERCEPTING ;
		   isFirstRun = true;
		   break ;
	   }
	   if(ForwardX(state->ballPos.x ) <  ForwardX(ourthreshX))
	   {
		   iState = DEFENDING ; 
		   break ;
	   }
	   cout<<"Positioning "<<endl;
            int predictPosX = ourthreshX  ;
			int predictPosY = state->ballPos.y - (state->ballPos.x - predictPosX)*tan(ang1);
		//	predictPosY = predictPosY + SGN(predictPosY - state->homePos[botID].y)*2*BOT_RADIUS;  //why ?
			if(state->ballVel.x < -100){
				sParam.GoToPointP.x = predictPosX;
				
				if(predictPosY > HALF_FIELD_MAXY - BOT_RADIUS)
					predictPosY = HALF_FIELD_MAXY - 2*BOT_RADIUS;
				else if( predictPosY < (-HALF_FIELD_MAXY + BOT_RADIUS))
					predictPosY = (-HALF_FIELD_MAXY + 2*BOT_RADIUS);
					
				cout<<" xpos : "<<predictPosX<<" ::  ypos : "<<predictPosY<<endl;	
				sParam.GoToPointP.y = predictPosY ; 
				sParam.GoToPointP.finalslope = PI/2;
				skillSet->executeSkill(SkillSet::GoToPoint, sParam);   
			}
			else
			{
				sParam.GoToPointP.x = predictPosX ;
				sParam.GoToPointP.y = state->ballPos.y  ;
				sParam.GoToPointP.finalslope = PI/2;
				skillSet->executeSkill(SkillSet::GoToPoint, sParam);
			}
			break ;
	   
	   
	}
	case INTERCEPTING :       // ball coming to our side from opp side 
	{
		if(ForwardX(state->ballPos.x) > ForwardX(oppthreshX) )
		{
		  iState = POSITIONING ;
		  isFirstRun = true;
		  break ;
		}
		if(ForwardX(state->ballPos.x )< ForwardX(ourthreshX ))
		{
		   iState = DEFENDING ;
		   isFirstRun = true;
		   break ;
		}
		cout<<" Intercepting "<<endl;
		sParam.SplineInterceptBallP.vl = 70;
		sParam.SplineInterceptBallP.vr = 70;
		if(isFirstRun){
			sParam.SplineInterceptBallP.initTraj=true;
			isFirstRun=false;
		}
		else sParam.SplineInterceptBallP.initTraj=false;
		skillSet->executeSkill(SkillSet::SplineInterceptBall , sParam);
		break;
	}
	case DEFENDING :           	// tAKES THE bot from behind
	{
	       if(ForwardX(state->ballPos.x ) > ForwardX(oppthreshX ))
		   {
			   iState = POSITIONING ;
			   break ;
		   }
			//for onot disturbing the covergoal 
			/*
			if(abs(state->ballPos.y) >= OUR_GOAL_MAXY + 2*BOT_RADIUS)
				{
					sID = SkillSet::GoToPoint;
					sParam.GoToPointP.align = false;
					sParam.GoToPointP.finalslope = 0;
			
					sParam.GoToPointP.x =state->ballPos.x	; // ..................changed
					sParam.GoToPointP.y = state->ballPos.y;
			       skillSet->executeSkill(SkillSet::GoToPoint,sParam) ;
				   break ;
			   }
			   else
			   {
			       sID = SkillSet::GoToPoint;
				   sParam.GoToPointP.x = -HALF_FIELD_MAXX + GOAL_DEPTH + 3*BOT_RADIUS;//can change this if cover goal collides
			       sParam.GoToPointP.finalslope = PI/2;
			       if(state->ballPos.y >0)
				     sParam.GoToPointP.y = OUR_GOAL_MAXY + BOT_RADIUS;
			       else
				     sParam.GoToPointP.y = OUR_GOAL_MINY - BOT_RADIUS;
			       skillSet->executeSkill(SkillSet::GoToPoint, sParam);
			      break;
				   
			   }
			*/
			
			//side spinning 
			if(distBotBall < BOT_BALL_THRESH)
            {
              if(fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 9 && fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 9)
              { 
				cout<<" Spinning"<<endl;
               if(state->ballPos.y > 0)
                iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;
               else
                iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;
               break ;
               } 
             } 
			 
			 // for ball in D-box 
			 if(abs(state->ballPos.y) < (OUR_GOAL_MAXY + 2*BOT_RADIUS))
			 {
				     cout<<"Defending in D-box_1 "<<endl;
					 sID = SkillSet::GoToPoint ;
					 sParam.GoToPointP.x = ForwardX(-(HALF_FIELD_MAXX-GOAL_DEPTH)*0.80 + 2*BOT_RADIUS);
                     sParam.GoToPointP.y = SGN(state->ballPos.y)*(HALF_FIELD_MAXY/2 + BOT_RADIUS);
					 skillSet->executeSkill(SkillSet::GoToPoint, sParam);
					 break ;
			 }
			 if(abs(state->ballPos.y)>OUR_GOAL_MAXY && ForwardX(state->ballPos.x) < -(HALF_FIELD_MAXX-GOAL_DEPTH)*0.80 + BOT_RADIUS)  //covergoal line
			 {
				 if(abs(state->ballPos.y) < abs(state->homePos[botID].y))
				 { 
					 cout<<"Defending in D-box_2 "<<endl;
					 sID = SkillSet::GoToPoint ;
					 sParam.GoToPointP.x = ForwardX(-(HALF_FIELD_MAXX-GOAL_DEPTH)*0.80 + 2*BOT_RADIUS);
                     sParam.GoToPointP.y = SGN(state->ballPos.y)*(HALF_FIELD_MAXY/2 + BOT_RADIUS);
					 skillSet->executeSkill(SkillSet::GoToPoint, sParam);
					 break ;
				 }
			 }
			
			 
			   cout<<"Defending"<<endl;
			   sID = SkillSet::GoToPoint;
               sParam.GoToPointP.align = true;
               float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
               float offset = 600;
               float factor = 0.00005;
               int targetX = state->ballPos.x + (int)distBotBall  * factor * avgBallVel.x;
               int targetY = state->ballPos.y + (int)distBotBall  * factor * avgBallVel.y;
               int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
               int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist;
               while(!isPointinField(Point2D<int>(x3, y3))) 
               {
                if(!isPointinField(state->ballPos))
                {
                 offset= 0;
                 x3 =  (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
                 y3 =  (targetY * (ballgoaldist + offset)) / ballgoaldist;
                 break;
                }
                offset /= 1.25;
                if(offset <= 1.0)
                  break;
                x3 =  (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
                y3 =  (targetY * (ballgoaldist + offset)) / ballgoaldist;
               }
               offset/=1.25;
               Vector2D<int> offsetpt(x3, y3);
               int dist2 = Vector2D<int>::dist(offsetpt, state->homePos[botID]);
               if(dist2 < 300)
                 hasAchievedOffset = 1;
		       else 
				   if(distBotBall  > 2 * offset)
                     hasAchievedOffset = 0; 

               if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x)) 
                hasAchievedOffset = 0;
            
               sParam.GoToPointP.x = x3;
               sParam.GoToPointP.y = y3;
               sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
               sParam.GoToPointP.increaseSpeed = 0;
               if(hasAchievedOffset)
               {
                 sParam.GoToPointP.x = state->ballPos.x;
                 sParam.GoToPointP.y = state->ballPos.y;
                 sParam.GoToPointP.finalslope = Vector2D<int>::angle( state->ballPos,state->homePos[botID]);

               }
		    	sParam.GoToPointP.align = false;
              if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x) && Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 1000) 
		        sParam.GoToPointP.align = true;
              skillSet->executeSkill(SkillSet::GoToPoint, sParam);
			  
			  break ;
		
	}
	case SPINNING_CCW :
	{
	  	if(distBotBall > 1.1*BOT_BALL_THRESH)
		{
			iState = DEFENDING;
			break;
		}
	
		sParam.SpinP.radPerSec = MAX_BOT_OMEGA;//for testing it is divided by 10
		skillSet->executeSkill(SkillSet::Spin, sParam);
		break;
	}
	case SPINNING_CW :
	{
		if(distBotBall > 1.1*BOT_BALL_THRESH)
		{
			iState = DEFENDING;
			break;
		}
		sParam.SpinP.radPerSec = -MAX_BOT_OMEGA;//for testing it is divided by 10
		skillSet->executeSkill(SkillSet::Spin, sParam);
		break;
	}
	

     }		
  }

	}; 
	}
#endif //TTDWDefender