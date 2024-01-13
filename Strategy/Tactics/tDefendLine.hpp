#ifndef TDEFENDLINE_HPP
#define TDEFENDLINE_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

namespace Strategy
{
  class TDefendLine : public Tactic
  {
  public:
    TDefendLine(const BeliefState* state, int botID) :
    Tactic(Tactic::DefendLine, state, botID)
    {
      iState = POSITION;
    } // TDefendLine

    ~TDefendLine()
    { } // ~TDefendLine

    enum InternalState
    {
      POSITION,
      CLOSE_TO_BALL,
      SPINNING_CCW,
      SPINNING_CW
    } iState;

    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
      int mindis = INT_MAX ,dist;
	  float a,b;
	  if( tParam->DefendLineP.x1 - tParam->DefendLineP.x2 == 0)
	  {
		  a=1; b=0;
	  }
	  else if(tParam->DefendLineP.y1 -tParam->DefendLineP.y2 == 0)
	  {
		  a=0; b=1;
	  }
	  else
	  { 
		  a= -(tParam->DefendLineP.y1 -tParam->DefendLineP.y2)/(tParam->DefendLineP.x1 - tParam->DefendLineP.x2);
		  b=1;
	  }
      //Vector2D<int>target((tParam->DefendLineP.x1+tParam->DefendLineP.x2)/2,(tParam->DefendLineP.y1+tParam->DefendLineP.y2)/2);

      for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        dist = abs(a*state->homePos[*it].x + b*state->homePos[*it].y);
        if(dist < mindis)
        {
          mindis = dist;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      if(tParam.DefendLineP.x1 == tParam.DefendLineP.x2)
	  {
		  sID = SkillSet::GoToPoint;
          sParam.GoToPointP.x = tParam.DefendLineP.x1 ;
          int temp = getBotDestPointY(tParam.DefendLineP.x1);
		  //checking the given condition 
		  if((abs(temp)>abs(tParam.DefendLineP.y1))&&(abs(temp)>abs(tParam.DefendLineP.y2))||((abs(temp)<abs(tParam.DefendLineP.y1))&&(abs(temp)<abs(tParam.DefendLineP.y2))))
			 temp = (abs(temp-tParam.DefendLineP.y1)>abs(temp - tParam.DefendLineP.y2))?(tParam.DefendLineP.y2):(tParam.DefendLineP.y1);
		   
			   
          sParam.GoToPointP.y = temp;
					printf("\n Predicting point = %d \n",temp);
          sParam.GoToPointP.align = false;
          sParam.GoToPointP.finalslope = -PI / 2;  
		   skillSet->executeSkill(sID, sParam);
	  }
	  else
	  {
	  
	  /*
	 switch(iState)
      {
        case POSITION:
          {
            getbotdest(tParam,&sParam.GoToPointP.x,&sParam.GoToPointP.y,&sParam.GoToPointP.finalslope);
            printf("after x = %f y -%f m = %f\n",sParam.GoToPointP.x,sParam.GoToPointP.y,sParam.GoToPointP.finalslope);
            sParam.GoToPointP.align = true;
            sID = SkillSet::GoToPoint;

            if( Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < BOT_BALL_THRESH*1.1 )
              iState = CLOSE_TO_BALL;
            break;
          }

        case CLOSE_TO_BALL:
        {
          if(fabs((float)normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 10 
          && fabs((float)normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 10
          && ForwardX(state->ballPos.x)>ForwardX(state->homePos[botID].x))
          {
            if(state->ballPos.y > 0)
              iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;
            else
              iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;
          }

          if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) > BOT_BALL_THRESH*1.1)
          {
            iState = POSITION;
          }

          break;
        }

        case SPINNING_CW:
          {
            sID = SkillSet::Spin;
            sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);

            if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) > BOT_BALL_THRESH*1.1)
            {
              iState = POSITION;
            }

            break;

          }

        case SPINNING_CCW:
        {
          sID = SkillSet::Spin;
          sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);

          if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) > BOT_BALL_THRESH*1.5)
          {
            iState = POSITION;
          }

          break;
        }

      }
      skillSet->executeSkill(sID, sParam);
	  */
	  }
	  }
    
    //from tcovergoal
	int getBotDestPointY(int posx)
    {
      Vector2D<int> ballFinalpos, botDestination, point;
      int flag=2;
      float balldistratio = fabs(state->ballPos.x)/(1*(posx))<1 ? fabs(state->ballPos.x)/(posx ):1 ;
      
      point.y = balldistratio*SGN(state->ballPos.y)*MIN(fabs(state->ballPos.y), OUR_GOAL_MAXY); 
        printf("\npoint.y = %d\n",point.y);
      /* Workaround for ball velocity 0*/
      if( ( ( fabs(state->ballVel.y) + fabs(state->ballVel.x) < 50) ) || (ForwardX(state->ballVel.x)<0 && ForwardX(state->ballVel.x)>(-50)) )
      {
       if(ForwardX(state->ballPos.x) > ( posx ))
        point.y = 0,flag=0;
      }
      else if(ForwardX(state->ballVel.x)>0 )
      {
        if(ForwardX(state->ballPos.x) > (posx))
          point.y = 0,flag =0;
      }
      else if (ForwardX(state->ballVel.x) <=(-50) )
      {
        if(ForwardX(state->ballPos.x) > (-HALF_FIELD_MAXX*0.8) )
        point.y = (state->ballVel.y/state->ballVel.x)*(posx - (state->ballPos.x)) + state->ballPos.y,flag = 1;
      }
      if(point.y > BOT_RADIUS) point.y -= 1.5*BOT_RADIUS;
      else if(point.y < -BOT_RADIUS) point.y += 1.5*BOT_RADIUS;
         //point.y = point.y + (state->ballVel.y > 0 ? BOT_RADIUS : -BOT_RADIUS);
        
      /* Set Limits on y to not exceed MAXY Limits*/
        if(abs(point.y)>HALF_FIELD_MAXY)
			point.y = SGN(point.y)*HALF_FIELD_MAXY*0.8 ;
        
		return point.y;
    }
	//
	
   /* void getbotdest(Param tParam,float *x,float *y,float *slope)
    {
      Vector2D<float>p1(tParam.DefendLineP.x1,tParam.DefendLineP.y1);
      Vector2D<float>p2(tParam.DefendLineP.x2,tParam.DefendLineP.y2);
      Vector2D<float>b(state->ballPos.x,state->ballPos.y);
      float m;
      
      //calculating x , y and slope required
      if(p2.x==p1.x) 
      {
        *x = p1.x ;
        *y = state->ballPos.y ;//+ 0.3*state->ballVel.y;
        *slope = PI/2;
      }
      else if(p2.y==p1.y)
      {
        *x = state->ballPos.x ;//+ 0.3*state->ballVel.x;
        *y = p2.y;
        *slope = 0;
      }
      else
      { 
        m = (p2.y-p1.y)/(p2.x-p1.x);      
        *x = ( m*(b.y-p1.y) + m*m*p1.x + b.x)/(1 + m*m);
        *y = m*(*x - p1.x) + p1.y;
        *slope = atan(m);
      }
      
      Vector2D<float>p(*x,*y);

      //setting limits on x and y
      if((*x-p1.x)*(*x-p2.x)>0 || (*y-p1.y)*(*y-p2.y)>0)
      {
        if(fabs(*x-p1.x)>abs(*x-p2.x) || fabs(*y-p1.y)>abs(*y-p2.y))
          *x = p2.x,*y = p2.y;
        else *x = p1.x,*y = p1.y;
      }
      
      printf("before x = %f y -%f m = %f\n",*x,*y,*slope);
      
    }*/
  }; // class TDefendLine
} // namespace MyStrategy

#endif // TDEFENDLINE_HPP