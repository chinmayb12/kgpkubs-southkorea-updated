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
  class PPositionOppPenalty : public Play
  {
    public: 
    inline PPositionOppPenalty(const BeliefState& state) : Play(state)
    {
      name = "PositionOppPenalty";
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      assert(HomeTeam::SIZE ==3);
      Tactic::Param param;
      param.PositionP.align = -PI;
	  
      /* Role 0 - goalkeeper*/
	/* param.PositionP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.8);
	 param.PositionP.y = 0;
	 param.PositionP.align = true;
	 param.PositionP.finalSlope = PI/2;
      */
	 roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
	  
  /* Role 1 - Position 1*/
	  param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = HALF_FIELD_MAXY/4;
      roleList[1].push_back(std::make_pair(Tactic::Position,param));
      
 /* Role 1 - Position 2*/
      param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = HALF_FIELD_MAXY/2;
      roleList[2].push_back(std::make_pair(Tactic::Position,param));
      
//  /* Role 1 - Position 3*/
//       param.PositionP.x =ForwardX(BOT_RADIUS);
// 	  param.PositionP.y = -HALF_FIELD_MAXY/2;
//       roleList[3].push_back(std::make_pair(Tactic::Position,param));
	  
//  /* Role 1 - Position 4*/
//       param.PositionP.x =ForwardX(BOT_RADIUS);
// 	  param.PositionP.y = -HALF_FIELD_MAXY/4;
//       roleList[4].push_back(std::make_pair(Tactic::Position,param));
      
      computeMaxTacticTransits();
    }
	
	virtual void reevaluateRoleParams()
    {
        {  
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[1][0].second;
           param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = HALF_FIELD_MAXY/4;
	  param.PositionP.finalSlope  = 0;
        }
        {
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[2][0].second;
            param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = HALF_FIELD_MAXY/2;
	  param.PositionP.finalSlope  = 0;
      
        }
        {   
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[3][0].second;
            param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = -HALF_FIELD_MAXY/2;
	 param.PositionP.finalSlope = 0;
			
        }
		{
			Vector2D<int> finalpoint;
		Tactic::Param &param = roleList[4][0].second;
		 param.PositionP.x =ForwardX(BOT_RADIUS);
	  param.PositionP.y = -HALF_FIELD_MAXY/4;
	  param.PositionP.finalSlope  = 0;
		}
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }
    inline ~PPositionOppPenalty()
    {
    }
	
    inline bool applicable(void) const
    {      
      if(state.pr_ourPenaltyKick)
        return true;
      return false;
    }
	
    inline Result done(void) const
    {

        return NOT_TERMINATED;
    }
  };
}