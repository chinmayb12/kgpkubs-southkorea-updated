#ifndef PSTOP_HPP
#define PSTOP_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"

/* This play positions the bots in the position in case we lose the toss and Opponent strikes the ball first. */
namespace Strategy
{
  class PStop : public Play
  {
  public:
    inline PStop(const BeliefState& state) 
      : Play(state)
    {
      name = "Stop";

		setTimeout(500,10);
		
      assert(HomeTeam::SIZE == 3); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      
      roleList[0].push_back(std::make_pair(Tactic::Stop, param));
      
;
      roleList[1].push_back(std::make_pair(Tactic::Stop, param));

      roleList[2].push_back(std::make_pair(Tactic::Stop, param));

      // roleList[3].push_back(std::make_pair(Tactic::Stop, param));
      
      // /* Always make one robot as goalie*/
      // roleList[4].push_back(std::make_pair(Tactic::Stop, param));
      
      computeMaxTacticTransits();
    }

    inline ~PStop()
    { }

    inline bool applicable(void) const
    {
      // printf("Set position is applicable\n");
      if(state.pr_ourFreeKick || state.pr_oppFreeKick || state.pr_ourGoalKick ||
              state.pr_oppGoalKick || state.pr_ourPenaltyKick || state.pr_oppPenaltyKick ||
                      state.pr_ourKickOff || state.pr_oppKickOff)
                        return false;
      if(!state.pr_gameRunning)
          return true;
      return false;
    }

    inline Result done(void) const
    {
      // TODO make it more sophisticated and also use the timeout info to determine if the play has terminated
      // printf("Done condition not finalised\n");
      return NOT_TERMINATED;
    }
  }; // class PSTOP
} // namespace Strategy

#endif // PSTOP_HPP
