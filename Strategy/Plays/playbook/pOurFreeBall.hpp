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
  class POurFreeBall : public Play
  {
    public: 
    inline POurFreeBall(const BeliefState& state) : Play(state)
    {
      name = "OurFreeBall";
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      assert(HomeTeam::SIZE ==3);
      Tactic::Param param;
       
	   setTimeout(4,10);
	   
	   /* Role 0 - for the goalie */
	  roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
	  
      /* Role 1 - Position 1*/
	 // param.PositionP.x = state.ballPos.x; param.PositionP.y = state.ballPos.y ;
      roleList[1].push_back(std::make_pair(Tactic::Shoot, param));
	
	 /* Role 1 - Position 2*/
      roleList[2].push_back(std::make_pair(Tactic::Stop,param));
     
	//  /* Role 2  - Shooter */
	//   //roleList[3].push_back(std::make_pair(Tactic::Shoot,param));
	//  	  roleList[3].push_back(std::make_pair(Tactic::Stop,param));
  //    /* Role 1 - Position 4*/
  //     roleList[4].push_back(std::make_pair(Tactic::Stop,param));
      
      computeMaxTacticTransits();
    }
	
          
    inline ~POurFreeBall()
    {
      
    }
    inline bool applicable(void) const
    {
      if(state.pr_ourFreeBall)
        return true;
      return false;
    }
    inline Result done(void) const
    {

        return NOT_TERMINATED;
    }
  };
}
