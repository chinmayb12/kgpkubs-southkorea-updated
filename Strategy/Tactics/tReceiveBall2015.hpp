#ifndef TRECEIVEBALL2015_HPP
#define TRECEIVEBALL2015_HPP
#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

namespace Strategy
{
  class TReceiveBall2015: public Tactic
  {
  public:
    TReceiveBall2015(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
			iState = POSITIONING;
		} // TReceiveBall
    ~TReceiveBall2015()
    { } // ~TReceiveBall
		
		enum InternalState
		{
			POSITIONING,
			APPROACH,
			REATTACK,
			CHARGE
		}iState;		//KD: I did not use any iState.
		
    inline bool isActiveTactic(void) const
    {
      return true;
    }
		
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      float minv = *(freeBots.begin());
      float mindis = 1e12;
			for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
			{
				const Vector2D<int> &botpos = state->homePos[*it];
				
				int dist = abs(state->homePos[*it].y + SGN(state->ballPos.y)*(HALF_FIELD_MAXY-2*BOT_RADIUS)/2);
				if(dist  < mindis)
				{
					mindis =  dist;
					minv = *it;
				}
			}
			cout<<" Receive :: "<<minv<<endl ;
			return minv;
    } // chooseBestBot)
    void execute(const Param& tParam)
    {
		Vector2D<int> dest;
		bool isDW = false;
		dest.x = state->ballPos.x - (3.5)*(state->ballPos.x - (HALF_FIELD_MAXX/2))/(HALF_FIELD_MAXX/2)*BOT_RADIUS; //0.5 as factor  dest.y =-SGN(state->ballPos.y)*HALF_FIELD_MAXY*0.4;//-SGN(state->ballPos.y)*1.5*BOT_RADIUS;
		dest.y = SGN(state->ballPos.y)*0.4*HALF_FIELD_MAXY*(-1);
			
		//**********************************************************************
		Vector2D<int> hit;
		Vector2D<int> oppGoal(HALF_FIELD_MAXX, 0);
		float n;
			
		if(state->ballVel.y < 200) n = 1.05;		//KD: Constants from simurosot. Must be changed after testing.
		else if(state->ballVel.y < 400) n = 1.1;
		else if(state->ballVel.y < 800) n = 1.2;
		else if(state->ballVel.y < 1000) n = 1.3;
		else if(state->ballVel.y < 1200) n = 1.4;
		else if(state->ballVel.y < 1600) n = 2.1;
		else if(state->ballVel.y < 1800) n = 2.2;
		else if(state->ballVel.y < 2000) n = 2.4;
		else if(state->ballVel.y < 2200) n = 2.6;
		else if(state->ballVel.y < 2500) n = 3.0;
		else if(state->ballVel.y < 2800) n = 3.2;
		else if(state->ballVel.y < 35000) n = 4;
		else n = 6;
			
		if( abs(state->ballVel.y) > 3000 && abs(state->ballVel.x) > 1800 && state->ballPos.x > HALF_FIELD_MAXX/2 + 2*BOT_RADIUS)
		{
			dest.x = HALF_FIELD_MAXX - GOAL_DEPTH - DBOX_WIDTH - BOT_RADIUS;
			dest.y = 0;
		}
		else if(((state->ballPos.y) > HALF_FIELD_MAXY - 1.6*BOT_RADIUS || (state->ballPos.y) < -HALF_FIELD_MAXY + 1.6*BOT_RADIUS) && state->ballVel.x > 800) 
		{
								
			dest.x = HALF_FIELD_MAXX - GOAL_DEPTH - DBOX_WIDTH - 2.5*BOT_RADIUS;
			dest.y = 0;
		}
			
		else if(state->ballPos.x > HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS /*&& abs(state->ballVel.x)<500*/ && abs(state->ballPos.y) > OPP_GOAL_MAXY - 0.5*BOT_RADIUS && abs(state->ballPos.y) < 5*BOT_RADIUS + OPP_GOAL_MAXY)
		{
			int id = 0;
			for(int i = 0; i < 4; i++)
			{
				if(state->awayPos[i].x > HALF_FIELD_MAXX - GOAL_DEPTH - 1.7*BOT_RADIUS && abs(state->awayPos[i].y) > OPP_GOAL_MAXY + BOT_BALL_THRESH*0.8)  
				{
					id = i;
					break;
				}
			}
			
			dest.x = HALF_FIELD_MAXX - GOAL_DEPTH - DBOX_WIDTH + BOT_RADIUS*1.1;
			if(state->awayPos[id].y > 0)
				dest.y = 1*BOT_RADIUS;
			else dest.y = -1*BOT_RADIUS; 
				//dest.y=0;

		}
		else if(state->ballPos.x > HALF_FIELD_MAXX - DBOX_WIDTH - GOAL_DEPTH - 8*BOT_RADIUS 
				/*&& abs(state->ballPos.y)<OPP_GOAL_MAXY +1*BOT_RADIUS && state->ballVel.x < 1000 && state->ballVel.y<1000*/ 
				&& state->ballPos.x > state->homePos[botID].x 
				&& /*(abs(state->ballPos.y)-abs(state->homePos[botID].y))*/abs(state->ballPos.y - state->homePos[botID].y) < n*BOT_RADIUS )
		{
			sID = SkillSet::DWGoToPoint;

			float factorx;
			if( state->ballVel.x < 200 )
				factorx = 0.000012;
			else if(state->ballVel.x < 500 )
				factorx = 0.000015;
			else if(state->ballVel.x < 800 )
				factorx = 0.000018;
			else if(state->ballVel.x < 1200 )
				factorx = 0.000026;
			else if(state->ballVel.x < 1400)
				factorx = 0.000045;
			else if(state->ballVel.x < 1800 )
				factorx = 0.000065;
			else if(state->ballVel.x < 2200 )
				factorx = 0.000090;
			else if(state->ballVel.x < 2600 )
				factorx = 0.000130;
			else if(state->ballVel.x < 3000 )
				factorx = 0.000150;
			else if(state->ballVel.x < 3400 )
				factorx = 0.000160;
			else if(state->ballVel.x < 3800 )
				factorx = 0.000180;
			else factorx = 0.0002;


			float factory;
			if( state->ballVel.y < 200 )
				factory = 0.000012;
			else if(state->ballVel.y < 500 )
				factory = 0.000015;
			else if(state->ballVel.y < 800 )
				factory = 0.000018;
			else if(state->ballVel.y < 1200 )
				factory = 0.000026;
			else if(state->ballVel.y < 1400)
				factory = 0.000045;
			else if(state->ballVel.y < 1800 )
				factory = 0.000065;
			else if(state->ballVel.y < 2200 )
				factory = 0.000090;
			else if(state->ballVel.y < 2600 )
				factory = 0.000130;
			else if(state->ballVel.y < 3000 )
				factory = 0.000150;
			else if(state->ballVel.y < 3400 )
				factory = 0.000160;
			else if(state->ballVel.y < 3800 )
				factory = 0.000180;
			else factory = 0.00020;

			
			float dist;
			dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
			dest.y = state->ballPos.y + state->ballVel.y*dist*factory;

			dest.x = state->ballPos.x + state->ballVel.x*dist*factorx + BOT_RADIUS*2;
			sParam.GoToPointP.align = false;
			sParam.DWGoToPointP.x = dest.x;
			sParam.DWGoToPointP.y = dest.y;
			sParam.DWGoToPointP.finalSlope = Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos);
			sParam.GoToPointP.increaseSpeed = 1;

			sParam.GoToPointP.finalVelocity = MAX_BOT_SPEED;
						
						
			}
			
			else if(state->ballPos.x > 0)
			{
				
				int id = 0;
				for(int i = 0; i < 4; i++)
				{
					if(state->awayPos[i].x > HALF_FIELD_MAXX - GOAL_DEPTH - 1.5*BOT_RADIUS && abs(state->awayPos[i].y) > OPP_GOAL_MAXY + BOT_BALL_THRESH*1.2)  
					{
						id = i;
						break;
					}
				}
				
				float m;
				m = state->ballVel.y/state->ballVel.x;
				
				if(state->awayPos[id].y < 0) dest.y = OPP_GOAL_MAXY - 1.8*BOT_RADIUS;
				else dest.y = OPP_GOAL_MINY + 1.8*BOT_RADIUS;
				
		
				if(m > 0 && state->ballVel.x > 0)
				{
					dest.x = state->ballPos.x + abs(state->ballPos.y + SGN(dest.y)*dest.y*-1)/m - BOT_RADIUS*3;
				}
				else if(m > 0 && state->ballVel.x < 0)
				{
					dest.x = state->ballPos.x - abs(state->ballPos.y + SGN(dest.y)*dest.y*-1)/m - BOT_RADIUS*2;

				}
				else if(m < 0 && state->ballVel.x < 0)
				{
					m = -m;
					dest.x = state->ballPos.x - abs(state->ballPos.y + SGN(dest.y)*dest.y*-1)/m - BOT_RADIUS*2;
				}
				else if(m < 0 && state->ballVel.x > 0)
				{
					m = -m;
					dest.x = state->ballPos.x + abs(state->ballPos.y + SGN(dest.y)*dest.y*-1)/m - BOT_RADIUS*3;
				}
				
				if(dest.x < 0) 
				{
					dest.x = HALF_FIELD_MAXX/2;
				}	
			}

/*			if(float distance = Vector2D<int>::dist(state->ballPos,state->homePos[botID])< 1.2*BOT_BALL_THRESH && state->ballPos.x > state->homePos[botID].x+BOT_RADIUS*0.1)
			{
				if(state->ballPos.y>state->homePos[botID].y)
				{
					sID = SkillSet::Spin;
					if(FIELD_IS_INVERTED == false)
						sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
					else
						sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);
					skillSet->executeSkill(sID, sParam);
					return;
				}
				else
				{
					sID = SkillSet::Spin;
					if(FIELD_IS_INVERTED == false)
						sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);
					else
						sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
             
					skillSet->executeSkill(sID, sParam);
					return;

				}
			}*/
		
			if (dest.x > HALF_FIELD_MAXX - GOAL_DEPTH - BOT_RADIUS) dest.x = HALF_FIELD_MAXX - GOAL_DEPTH - BOT_RADIUS*0.3; 

	   
			sID = SkillSet::GoToPoint;
			sParam.GoToPointP.y = dest.y;
			sParam.GoToPointP.x = dest.x;
								
			sParam.GoToPointP.align = true;
			sParam.GoToPointP.finalslope = Vector2D<int>::angle(oppGoal, state->homePos[botID]);

			skillSet->executeSkill(sID, sParam);
		}
  } ;// class TReceiveBall
} // namespace Strategy
#endif // TRECEIVEBALL_HPP