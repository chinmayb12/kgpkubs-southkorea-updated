#pragma once
#include <utility>
#include "play.hpp"
#include "kalman.h"
#include "beliefState.h"
#include "tactic.h"
#include "config.h"
using namespace std;

namespace Strategy
{
  class POppFreeBall : public Play
  {
    public: 
    inline POppFreeBall(const BeliefState& state) : Play(state)
    {
      name = "OppFreeBall";
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      assert(HomeTeam::SIZE ==3);
      Tactic::Param param;
       
	   setTimeout(2,10);
	   
	   /* Role 0 - for the goalie */
	  roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
	  
      /* Role 1 - Position 1*/
	  roleList[1].push_back(std::make_pair(Tactic::Stop,param));
      
      /* Role 2  - Shooter */
	  roleList[2].push_back(std::make_pair(Tactic::Stop,param));
      
//  /* Role 1 - Position 3*/
//       roleList[3].push_back(std::make_pair(Tactic::Stop,param));
	  
//  /* Role 1 - Position 4*/
//       roleList[4].push_back(std::make_pair(Tactic::Stop,param));
      
      computeMaxTacticTransits();
    }
          
    inline ~POppFreeBall()
    {
      
    }
    inline bool applicable(void) const
    {
      
      if(state.pr_ourPenaltyKickStart)
        return true;
      return false;
    }
    inline Result done(void) const
    {

        return NOT_TERMINATED;
    }
  };
}
