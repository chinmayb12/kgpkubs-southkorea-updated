#ifndef TDEFENDPOINT_HPP
#define TDEFENDPOINT_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
namespace Strategy
{
    class TDefendPoint : public Tactic
    {
    public:
        TDefendPoint(const BeliefState* state, int botID) :
            Tactic(Tactic::Stop, state, botID)
        {
          iState = BLOCKING;
        } // TDefendPoint

        ~TDefendPoint()
        { } // ~TDefendPoint
    enum InternalState
    {
      SPINNING_CCW = -1,
      SPINNING_CW = 1,
      BLOCKING
    } iState;

    inline bool isActiveTactic(void) const
    {
        return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
        int minv = *(freeBots.begin());
        float mindis = 10000000;
        for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
        {
           // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
          float dist = Vector2D<int>::dist(Vector2D<int>(tParam->DefendPointP.x,tParam->DefendPointP.y),state->homePos[*it]);
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
      printf("DefendPoint BotID: %d\n",botID);
      
      switch(iState)
      {

        case BLOCKING:
        {
          //printf("BLOCKING\n");
          //printf("\n%f\n",Vector2D<int>::dist(state->ballPos, state->homePos[botID]));
          // Select the skill to the executed next
          sID = SkillSet::DefendPoint;
          sParam.DefendPointP.x=tParam.DefendPointP.x;
          sParam.DefendPointP.y=tParam.DefendPointP.y;
          sParam.DefendPointP.radius=tParam.DefendPointP.radius;
          
          if (Vector2D<int>::dist(state->ballPos, state->homePos[botID]) < 1.5*BOT_BALL_THRESH) 
          {
            if(state->ballPos.y > 0)
              iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;
            else
              iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;
          }
          
          break;
        }

        case SPINNING_CCW:
        {
          //printf("SPINNING_CCW\n");
          sID = SkillSet::Spin;
          sParam.SpinP.radPerSec = (-MAX_BOT_OMEGA);

          if (Vector2D<int>::dist(state->ballPos, state->homePos[botID]) > 1.5*BOT_BALL_THRESH)
          {
            iState = BLOCKING;
          }

          break;
        }

        case SPINNING_CW:
        {
          //printf("SPINNING_CW\n");
          sID = SkillSet::Spin;
          sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);

          if (Vector2D<int>::dist(state->ballPos, state->homePos[botID]) > 1.5*BOT_BALL_THRESH)
          {
            iState = BLOCKING;
          }

          break;
        }
      }

      skillSet->executeSkill(sID, sParam);
    }
  };// class TDefendPo9nt
} // namespace Strategy

#endif // TDEFENDPOINT_HPP