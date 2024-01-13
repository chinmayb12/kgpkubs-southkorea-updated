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
  class POurFreeKick : public Play
  {
    public: 
    inline POurFreeKick(const BeliefState& state) : Play(state)
    {
      name = "OurFreeKick ";
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      assert(HomeTeam::SIZE ==3);
      Tactic::Param param;
       
	   setTimeout(4,10);
	   
	    /* Role 0 - for the goalie */
	  roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
	  
      /* Role 1 - Position 1*/
	  roleList[1].push_back(std::make_pair(Tactic::Shoot,param));
      
	 /* Role 1 - Position 2*/
      roleList[2].push_back(std::make_pair(Tactic::Stop,param));
     
	//  /* Role 2  - Shooter */
	//   roleList[3].push_back(std::make_pair(Tactic::Stop,param));
	  
  //    /* Role 1 - Position 4*/
  //     roleList[4].push_back(std::make_pair(Tactic::Stop,param));
      
      computeMaxTacticTransits();
    }
	virtual void reevaluateRoleParams()
	{
	    {
		  Tactic::Param &param = roleList[0][0].second;
		}
		{
			Tactic::Param &param = roleList[1][0].second;
		}
		{
			Tactic::Param &param = roleList[2][0].second;
		}
		{
			Tactic::Param &param = roleList[3][0].second;
		}
		{
			Tactic::Param &param = roleList[4][0].second;
		}
	}
          
    inline ~POurFreeKick ()
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
