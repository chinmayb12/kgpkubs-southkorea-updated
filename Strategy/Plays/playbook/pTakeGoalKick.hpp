#ifndef PTAKEGOALKICK_HPP
#define PTAKEGOALKICK_HPP

#include <utility>
#include "play.hpp"
#include "kalman.h"
#include "beliefState.h"
#include "tactic.h"
#include "config.h"
using namespace std;

namespace Strategy
{
  class PTakeGoalKick : public Play
  {
    public: 
    inline PTakeGoalKick(const BeliefState& state) : Play(state)
    {
      name = "TakeOurGoalKick";
      assert(HomeTeam::SIZE ==3);
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
      /* Role 0 - Goalie */
			roleList[0].push_back(std::make_pair(Tactic::GoalKick,param));
      
      /* Role 1 - in our half position1*/
       roleList[1].push_back(std::make_pair(Tactic::Stop,param));
      
      /* Role 2 -in our half position 2 */
      roleList[2].push_back(std::make_pair(Tactic::Stop,param));
      
      // /* Role 3 - Bot1 ready to recieve ball in opp half*/
      // roleList[3].push_back(std::make_pair(Tactic::Stop,param));
      
      // /* Role 4 - Midfield Player */
      // roleList[4].push_back(std::make_pair(Tactic::Stop, param));
      
      computeMaxTacticTransits();
    }
          
    inline ~PTakeGoalKick()
    {
      
    }
    inline bool applicable(void) const
    {
      //if(state.pr_ourGoalKick)
      //  return true;
       return false;
    }
    inline Result done(void) const
    {
      return NOT_TERMINATED;
    }
  };
}
#endif // PTAKE_GOAL_KICK_HPP
