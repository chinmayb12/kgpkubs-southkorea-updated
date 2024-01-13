#ifndef POURGOALKICK_HPP
#define POURGOALKICK_HPP

#include <utility>
#include "play.hpp"
#include "kalman.h"
#include "beliefState.h"
#include "tactic.h"
#include "config.h"
using namespace std;

namespace Strategy
{
  class POurGoalKick : public Play
  {
    public: 
    inline POurGoalKick(const BeliefState& state) : Play(state)
    {
      name = "OurGoalKick";
      assert(HomeTeam::SIZE ==3);
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
			setTimeout(5);
			//Role 0 Goal keeper
			
			roleList[0].push_back(std::make_pair(Tactic::GoalKick, param));
			roleList[1].push_back(std::make_pair(Tactic::Stop, param));
			roleList[2].push_back(std::make_pair(Tactic::Stop, param));
			// roleList[3].push_back(std::make_pair(Tactic::Stop, param));
			// roleList[4].push_back(std::make_pair(Tactic::Stop, param));

      computeMaxTacticTransits();
    }

    inline ~POurGoalKick()
    {
      
    }
    inline bool applicable(void) const
    {
      if(state.pr_ourGoalKick)
        return true;
       return false;
    }
    inline Result done(void) const
    {
      return NOT_TERMINATED;
    }
  };
}
#endif // POPP_GOAL_KICK_HPP
