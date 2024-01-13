#ifndef TRECEIVESPLINE_HPP
#define TRECEIVESPLINE_HPP
#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

namespace Strategy
{
  class TReceiveSpline: public Tactic
  { 
	 int splin ; 
  public:
    TReceiveSpline(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {  
		    splin = 0 ;
			iState = POSITIONING;
		} // TReceiveBall
    ~TReceiveSpline()
    { } // ~TReceiveBall
		
		enum InternalState
		{
			POSITIONING,
            ATTACKING
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
	  mindis1 = INT_MIN;
	  attackerSide = SGN(state->homePos[attacker].y);
	  for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
	  {
		  dist = Vector2D<int>::dist(state->homePos[*it],state->homePos[attacker]);
		  if(SGN(state->homePos[*it].y) == -attackerSide && state->homePos[*it].x >mindis1)
		  {
			  mindis1 =dist;
			  minv1 = *it;
		  }
		  else if(*it != attacker && dist < mindis2)
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
		 cout<<"Receiver :: "<<botID<<endl;
      switch(iState)
      {
		 
                                
        case POSITIONING:
        {
			if(abs(state->ballPos.y) < OUR_GOAL_MAXY + 2*BOT_RADIUS)
			{   
				if(state->ourBotNearestToBall==botID)
				{  
				   iState = ATTACKING ;
				   break ;
				}
				if((state->ourBotNearestToBall!=botID)&&(Vector2D<int>::dist(state->homePos[state->ourBotNearestToBall],state->ballPos) > 2*BOT_BALL_THRESH))
				{
				  iState = ATTACKING ;
				  break ;
				}	
			}
					cout<<"POSITIONG "<<endl ;
					Vector2D<int>Goal(OPP_GOAL_X,0);
					sID = SkillSet::GoToPoint;
					sParam.GoToPointP.y = -SGN(state->ballPos.y)*(HALF_FIELD_MAXY-2*BOT_RADIUS)/2;
					if(ForwardX(state->ballPos.x) > HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS )
					sParam.GoToPointP.x =  ForwardX(HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS) ;
					else
					sParam.GoToPointP.x = state->ballPos.x ;
				//	sParam.GoToPointP.x = botdpointx(sParam.GoToPointP.y,sParam.GoToPointP.finalslope) + BOT_RADIUS;
					if(ForwardX(state->ballPos.x) < (-HALF_FIELD_MAXX / 2 - BOT_RADIUS  ))
                    { 
                          sID=SkillSet::GoToPoint;
                          sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX /4) ;
						  sParam.GoToPointP.y =  SGN(state->ballPos.y)*( OUR_GOAL_MAXY  );
						  splin = 0 ;    
                          skillSet->executeSkill(sID, sParam);
                          break;
                    } 
					if(ForwardX(state->ballPos.x) > (HALF_FIELD_MAXX - GOAL_DEPTH - DBOX_WIDTH))
					{
						  sID=SkillSet::GoToPoint;
                          sParam.GoToPointP.x = ForwardX(HALF_FIELD_MAXX - GOAL_DEPTH - DBOX_WIDTH) ;
						  sParam.GoToPointP.y =  SGN(state->ballPos.y)*( OUR_GOAL_MAXY  );
						  splin = 0 ;    
                          skillSet->executeSkill(sID, sParam);
                          break;
						
					}	
				    SkillSet::addCircle(sParam.GoToPointP.x,sParam.GoToPointP.y,50, 0xFFF000);
					sParam.GoToPointP.finalslope = Vector2D<int>::angle(Goal,state->homePos[botID]);
					sParam.GoToPointP.align = true;
					char debug[100];
					sprintf(debug,"our ball %d\n",state->pr_ourBall);
					 skillSet->executeSkill(sID,sParam);

					break;
        }
        case ATTACKING :
		{ 
			if(abs(state->ballPos.y) > OUR_GOAL_MAXY + 2*BOT_RADIUS)
			{  
			   iState = POSITIONING ;
			    splin = 0 ;			   
			   break ;
			}
			if((abs(state->ballPos.y) < OUR_GOAL_MAXY + 2*BOT_RADIUS)&&((state->ourBotNearestToBall!=botID)&&(Vector2D<int>::dist(state->homePos[state->ourBotNearestToBall],state->ballPos) < 2*BOT_BALL_THRESH)))
		    {
			   iState = POSITIONING ; 
			   splin = 0 ;
			   break ;
			}
			
			if(ForwardX(state->homePos[botID].x) < ForwardX(state->ballPos.x )&& Vector2D<int>::dist(state->homePos[botID],state->ballPos)< 1.5*BOT_BALL_THRESH)
			{	
             //CLOSE TO BALL 
			 cout<<"DRAGGING "<<endl ;
			 sID = SkillSet::GoToPoint;
			 sParam.GoToPointP.x = OPP_GOAL_X;
             sParam.GoToPointP.y = 0 ; 
			 sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
			 sParam.GoToPointP.align = false ;
             splin = 0 ; 
			 skillSet->executeSkill(sID, sParam);
			 break ;
             
            } 
			cout<<"ATTACKING "<<endl ; 
	        sID = SkillSet::SplineInterceptBall;
            sParam.SplineInterceptBallP.vl = 100;
            sParam.SplineInterceptBallP.vr = 100;
            sParam.SplineInterceptBallP.velGiven = 1;
            sParam.SplineInterceptBallP.ballVelX = state->ballVel.x;
            sParam.SplineInterceptBallP.ballVelY = state->ballVel.y;
            if(splin == 0){
		      splin = 1;
            sParam.SplineInterceptBallP.initTraj = 1;
			} 
			else{
            splin = 1;
            sParam.SplineInterceptBallP.initTraj = 0;
            }
            sParam.SplineInterceptBallP.changeSpline = true;
            skillSet->executeSkill(sID, sParam);
            splin  = 1; 
	  
      break;
		 
	     }
	   }
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