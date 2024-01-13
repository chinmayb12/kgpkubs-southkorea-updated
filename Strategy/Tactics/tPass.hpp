#ifndef TTPASS_HPP
#define TTPASS_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#define D_TPASS 1
namespace Strategy
{
  class TPass : public Tactic
  {
  public:
    TPass(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {} // TPass

    ~TPass()
    { } // ~TPass

    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
			/*Arpit: tactic explicitly for Kickoff. chooses the farthest bot on the other side of the field. */
      int maxv = *(freeBots.begin());
      int maxdis = INT_MIN;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {        
        float dist = ForwardX(state->homePos[*it].x);
        if(dist > maxdis)
        {
          maxdis= dist;
          maxv = *it;
        }
      }
      //debug(1, maxv, "assigned Pass");
      return maxv;
    } // chooseBestBot
    void execute(const Param& tParam)
    {
			if(tState == Tactic::COMPLETED)
			{
				sID = SkillSet::Stop;
				skillSet->executeSkill(sID, sParam);
				return;
			}
      printf("Pass BotID: %d\n",botID);
      //tState = RUNNING;
			if(ForwardX(state->ballPos.x) < -150 || ForwardX(state->homePos[botID].x) <= 0)
				tState = Tactic::COMPLETED;
			sID = SkillSet::Velocity;
			int sgn = (FIELD_IS_INVERTED)?(-1):(1);
			sParam.VelocityP.vl = sgn*40;
			sParam.VelocityP.vr = sgn*40;
			

//			double theta = atan2(state->ballPos.y-state->homePos[botID].y, state->ballPos.x-state->homePos[botID].x);
//			sID = SkillSet::GoToPoint ;
//			sParam.GoToPointP.x = state->ballPos.x + 200*cos(theta);
//			sParam.GoToPointP.y = state->ballPos.y + 200*cos(theta);
//			sParam.GoToPointP.finalVelocity = 60;
//			sParam.GoToPointP.finalslope = theta;
			skillSet->executeSkill(sID, sParam);		
      //gotoPointExact(state->ballPos.x, state->ballPos.y, false, 0, MAX_BOT_SPEED);
    }
  };// class TPass
} // namespace Strategy

#endif // TTPass_HPP
