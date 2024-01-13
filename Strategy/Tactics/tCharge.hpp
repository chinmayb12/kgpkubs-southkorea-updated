#ifndef TTCHARGE_HPP
#define TTCHARGE_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

//TODO: Make a tactic such as tStealAndShoot which steals the ball, then shoots it, and
//      continues the stealing a few times if a goal isn't scored.

namespace Strategy
{
  class TCharge : public Tactic
  {
  public:
    TCharge(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { 
      iState =  POSITION;
    } // TCharge

    ~TCharge()
    { } // ~TCharge
    enum InternalState
    {
      POSITION,
      CHARGE,
    }iState;
    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
      int mindis = Vector2D<int>::dist(state->homePos[minv], state->ballPos);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        if(ForwardX(state->homePos[*it].x) < ForwardX(state->ballPos.x) && Vector2D<int>::dist(state->homePos[*it], state->ballPos) < mindis)
        {
          mindis = Vector2D<int>::dist(state->homePos[*it], state->ballPos);;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot
  
  int findOpportunity(float slope,float yProjection)
  {
    if(fabs(yProjection)>HALF_FIELD_MAXY)
      return 0;
    for(int i=yProjection-10;i<=yProjection+10;i+=2)
    {
      float y=state->ballPos.y-(yProjection-i);
      Vector2D<int> pt(state->ballPos.x,y);
      for(int i=0;i<5;i++) 
      {
      if((intersects(state->homePos[botID],pt,state->awayPos[i],(int)(2*BOT_RADIUS)))==true)
        return 0;
      }
    }
      return 1;
  }


    void execute(const Param& tParam)
    {
      printf("Charge BotID: %d\n",botID);

       /* sID = SkillSet::GoToBall;
        sParam.GoToBallP.align = true;*/
    //skillSet->executeSkill(sID, sParam);
    
   // float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
      switch(iState)
      {
    case POSITION:
           { 
						 double slope=0;
           sID = SkillSet::GoToPoint;
           sParam.GoToPointP.align = true;
           sParam.GoToPointP.x=0;
             slope=state->ballPos.y*1.0/(state->ballPos.x-HALF_FIELD_MAXX);
					 cout<<"slope= "<<slope<<endl;
           float yProjection=slope*-1*(HALF_FIELD_MAXX);
           if(fabs(yProjection)>HALF_FIELD_MAXY)
             sParam.GoToPointP.y=yProjection>0?HALF_FIELD_MAXY:-1*HALF_FIELD_MAXY;
             else
               sParam.GoToPointP.y=yProjection;
           sParam.GoToPointP.increaseSpeed = false;
            sParam.GoToPointP.finalslope = -slope;
           skillSet->executeSkill(sID, sParam);
          if(findOpportunity(slope,yProjection) && (fabs(state->homePos[botID].x)<HALF_FIELD_MAXX/16) && fabs(state->homePos[botID].y-yProjection)<=BOT_POINT_THRESH);
            iState=CHARGE;
        break;
           }
        case CHARGE : 
        {
           //printf("\n ATTACK \n");
            sID = SkillSet::GoToBall;
            /*sParam.GoToPointP.align = true;
            sParam.GoToPointP.x = OPP_GOAL_X;
            sParam.GoToPointP.y = 0;
            sParam.GoToPointP.finalslope = Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos);
            sParam.GoToPointP.increaseSpeed = 1;*/
            skillSet->executeSkill(sID, sParam);
						if(ForwardX(state->ballPos.x)<0 || ForwardX(state->ballPos.x)<ForwardX(state->homePos[botID].x))
							iState=POSITION;
          break;
        }
      }
      if (state->pr_ourGoalScored)
        tState = COMPLETED;
      else
        tState = RUNNING;
    }
  };
// class TCharge
} // namespace Strategy

#endif // TTCharge_HPP