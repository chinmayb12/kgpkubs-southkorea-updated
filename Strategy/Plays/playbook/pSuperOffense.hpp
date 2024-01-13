#ifndef PSUPER_OFFENSE_PLAY_HPP
#define PSUPER_OFFENSE_PLAY_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#include "config.h"

namespace Strategy
{
  class PSuperOffensePlay : public Play
  {
    public: 
    inline PSuperOffensePlay(const BeliefState& state) : Play(state)
    {
      name = "SuperOffense";
      assert(HomeTeam::SIZE ==3);
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
      /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 2 - Striker */
			param.AttackP.rotateOnError = true;
      roleList[1].push_back(std::make_pair(Tactic::AttackSpline,param));
      
      /* Role 3 - Defender */
      param.BlockP.dist = 1000;
      if(state.pr_ball_in_opp_dbox)
        roleList[2].push_back(std::make_pair(Tactic::ChargeBall,param));
      else
        roleList[2].push_back(std::make_pair(Tactic::Block,param));
      
      // /* Role 4 - Forward Support Player 1 */
      // roleList[3].push_back(std::make_pair(Tactic::ReceiveSpline,param));
      
      // /* Role 5 - Forward Support Player 2*/
      // roleList[4].push_back(std::make_pair(Tactic::CoverGoal2015,param));
      
      computeMaxTacticTransits();
    }
    
    inline ~PSuperOffensePlay()
    {
      
    }
    
    inline bool applicable(void) const
    {
			if(!state.pr_gameRunning)
        return false;
			/*
			int diff = state.oppGoalCount - state.ourGoalCount;
			if((diff >= 3 && state.pr_ballOppSide) || (diff <= -3 && state.pr_ballOppSide))
			*/
      if(state.pr_ball_in_opp_dbox)
        return true;
      return false;
    }
    
    inline Result done(void) const
    {
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(!state.pr_ball_in_opp_dbox) return FAILURE_LOW;
      return NOT_TERMINATED;
    }
  };
}
#endif // PSUPER_OFFENSE_PLAY_HPP