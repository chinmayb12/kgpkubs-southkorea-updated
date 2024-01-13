#ifndef PDEFENSE1_PLAY_HPP
#define PDEFENSE1_PLAY_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#include "config.h"

namespace Strategy
{
  class PDefense1Play : public Play
  {
    public: 
    inline PDefense1Play(const BeliefState& state) : Play(state)
    {
      name = "Defense1";
      assert(HomeTeam::SIZE ==3);
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
      /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 2 - Covergoal */
	  roleList[1].push_back(std::make_pair(Tactic::CoverGoal2015,param));
	
	  /* Role 3 - Defender */
	  roleList[2].push_back(std::make_pair(Tactic::DWDefender2015,param));
	  
	  // /* Role 4 - Block */
	  // roleList[3].push_back(std::make_pair(Tactic::Block,param));
	  
	  // /* Role 5 - Striker */	
		// param.AttackP.rotateOnError = true;
    //   roleList[4].push_back(std::make_pair(Tactic::AttackSpline,param));
      
     
      
      computeMaxTacticTransits();
    }
    
    inline ~PDefense1Play()
    {
      
    }
    
    inline bool applicable(void) const
    {
			if(!state.pr_gameRunning)
        return false;
				
			/*int diff = state.oppGoalCount - state.ourGoalCount;
      if(diff <=3 &&( state.pr_nOpponentsOurSide(1) && state.pr_ballOurSide && !state.pr_nOpponentsOurSide(3) ))
        return true;*/
      
      if(state.pr_ballOurSide && !state.pr_ball_in_our_dbox) 
        return true;
      return false;
    }
    
    inline Result done(void) const
    {
      if(state.pr_ballOppSide) return SUCCESS_LOW;
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(state.pr_ourBall && state.pr_ballOppSide) return SUCCESS_LOW;
      if(state.pr_ball_in_opp_dbox) return SUCCESS_MED;
      if(state.pr_ball_in_our_dbox) return FAILURE_MED;
      return NOT_TERMINATED;
    }
  };
}
#endif // PDEFENSE1_PLAY_HPP