#pragma once

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"

/* This play positions the bots in the position in case its our Free Kick*/
namespace Strategy
{
  class PKickoff : public Play
  {
  public:
    inline PKickoff(const BeliefState& state) 
      : Play(state)
    {
      name = "PKickOff";

      assert(HomeTeam::SIZE == 3);
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;     
			
			setTimeout(8,10);
			
			/* Role 0 - Passer */
      roleList[0].push_back(std::make_pair(Tactic::Kickoff,param));
      
      /* Role 1 - Stop*/
      roleList[1].push_back(std::make_pair(Tactic::AttackSpline,param));
      
			/* Role 2 - stop */
      roleList[2].push_back(std::make_pair(Tactic::Stop,param));
      
			// /* Role 3 - stop */
      // roleList[3].push_back(std::make_pair(Tactic::Stop,param));
      
			// /* Role 4 - stop*/
      // roleList[4].push_back(std::make_pair(Tactic::Stop,param));
      
      computeMaxTacticTransits();
    }
    
    inline ~PKickoff()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_ourKickOffStart)
        return true;
      return false;
    }

    inline Result done(void) const
    {
      /*Adding code here even though this should be handled by tactic completion. */			
      // printf("Done condition not finalised\n");
      return NOT_TERMINATED;
    }
  }; // class PKickoff
} // namespace Strategy
