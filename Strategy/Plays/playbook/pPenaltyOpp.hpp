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
  class PPenaltyOpp : public Play
  {
    public: 
    inline PPenaltyOpp(const BeliefState& state) : Play(state)
    {
      name = "SetOppPenaltyKick";
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_NO;
      assert(HomeTeam::SIZE ==3);
      Tactic::Param param;
      param.PositionP.align = -PI;
	  
	  setTimeout(3,10);
	  
      /* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - Position 1*/
	  param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = HALF_FIELD_MAXY/4;
      roleList[1].push_back(std::make_pair(Tactic::Position,param));
      
      /* Role 2 -Position 2*/
      param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = HALF_FIELD_MAXY/2;
      roleList[2].push_back(std::make_pair(Tactic::Position,param));
      
    //   /* Role 3 - Position 3*/
    //   param.PositionP.x =ForwardX(BOT_RADIUS);
	  // param.PositionP.y = -HALF_FIELD_MAXY/2;
    //   roleList[3].push_back(std::make_pair(Tactic::Position,param));
     
    //   /* Role 4 - Position  4 */
    //   param.PositionP.x =ForwardX(BOT_RADIUS);
	  // param.PositionP.y = -HALF_FIELD_MAXY/4;
    //   roleList[4].push_back(std::make_pair(Tactic::Position,param));
      
      computeMaxTacticTransits();
    }
          
    inline ~PPenaltyOpp()
    {
      
    }
    inline bool applicable(void) const
    {
      
      if(state.pr_oppPenaltyKick)
        return true;
      return false;
    }
    inline Result done(void) const
    {

        return NOT_TERMINATED;
    }
  };
}
