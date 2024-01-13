#ifndef POFFENSIVE2_PLAY_HPP
#define POFFENSIVE2_PLAY_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#include "config.h"

namespace Strategy
{
  class POffense2Play : public Play
  {
    public: 
    inline POffense2Play(const BeliefState& state) : Play(state)
    {
      name = "Offense2";
      assert(HomeTeam::SIZE == 3);
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
	   /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
	  roleList[1].push_back(std::make_pair(Tactic::CoverGoal2015,param));
	  
      /* Role 2 - Striker */
			param.AttackP.rotateOnError = true;
      roleList[2].push_back(std::make_pair(Tactic::AttackSpline,param));

      // roleList[3].push_back(std::make_pair(Tactic::ReceiveSpline,param));
      
      // roleList[4].push_back(std::make_pair(Tactic::SupportAttack2015,param));
	  
	  


        
      computeMaxTacticTransits();
    }
    
    inline ~POffense2Play()
    {
      
    }
    
    inline bool applicable(void) const
    {
//      if(!state.pr_nOpponentsOurSide(1) && state.pr_ballOurSide)
//        return true;
//      if(state.pr_ballOppSide)
//        return true;
      if(!state.pr_gameRunning)
        return false;
      if(state.pr_ballOppSide) 
        return true;
      return false;
    }
    
    inline Result done(void) const
    {
      if(state.pr_ballOurSide) return FAILURE_LOW;
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(state.pr_ball_in_opp_dbox) return SUCCESS_MED;
      return NOT_TERMINATED;
    }
  };
}
#endif // POFFENSIVE2_PLAY_HPP