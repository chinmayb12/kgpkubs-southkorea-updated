#ifndef TTSTOP_HPP
#define TTSTOP_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

namespace Strategy
{
  class TStop : public Tactic
  {
  public:
    TStop(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TTStop

    ~TStop()
    { } // ~TTStop

    inline bool isActiveTactic(void) const
    {
      return true;
    }

  int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int botid = *freeBots.begin();
      debug(1, botid , "assigned Stop");
      return botid;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      //printf("Stop BotID: %d\n",botID);
      // Select the skill to the executed next
      stopBot();

/*      if (state->homeVel[botID].absSq() < ZERO_VELOCITY_THRESHOLD*ZERO_VELOCITY_THRESHOLD)
      {
        tState = COMPLETED;
      }*/
    }
  }; // class TTStop
} // namespace Strategy

#endif // TTSTOP_HPP
