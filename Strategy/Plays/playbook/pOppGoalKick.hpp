#ifndef POPPGOALKICK_HPP
#define POPPGOALKICK_HPP

#include <utility>
#include "play.hpp"
#include "kalman.h"
#include "beliefState.h"
#include "tactic.h"
#include "config.h"
using namespace std;

namespace Strategy
{
  class POppGoalKick : public Play
  {
    public: 
    inline POppGoalKick(const BeliefState& state) : Play(state)
    {
      name = "OppGoalKick";
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_NO;
      assert(HomeTeam::SIZE ==3);
      Tactic::Param param;
      
	  setTimeout(3,10);
	  
      /* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - Defender 1*/
      roleList[1].push_back(std::make_pair(Tactic::CoverGoal,param));
      
      /* Role 2 -Defender 2*/
      param.PositionP.x = ForwardX(-BOT_RADIUS);
	  param.PositionP.y = state.ballPos.y ;
	  param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position,param));
      
      // /* Role 3 - Bot1 ready to intercept ball */
      // param.DefendLineP.x1 = ForwardX(-8*BOT_RADIUS);
      // param.DefendLineP.y1 = HALF_FIELD_MAXY;
      // param.DefendLineP.x2 = ForwardX(-8*BOT_RADIUS);
      // param.DefendLineP.y2 = 0;//HALF_FIELD_MAXY;
      // roleList[3].push_back(std::make_pair(Tactic::DefendLine,param));

      // /* Role 4 - Midfield Player */
      // param.DefendLineP.x1 = ForwardX(-4*BOT_RADIUS);
      // param.DefendLineP.y1 = -HALF_FIELD_MAXY;
      // param.DefendLineP.x2 = ForwardX(-4*BOT_RADIUS);
      // param.DefendLineP.y2 = 0;//HALF_FIELD_MAXY;
      // roleList[4].push_back(std::make_pair(Tactic::DefendLine, param));
      
      computeMaxTacticTransits();
    }
	virtual void reevaluateRoleParams()
    {
        
        {
			
             Tactic::Param &param = roleList[2][0].second;
             param.PositionP.x =ForwardX(-BOT_RADIUS);
             param.PositionP.y = state.ballPos.y;
             param.PositionP.finalSlope  = 0;
        }
	}
          
    inline ~POppGoalKick()
    {
      
    }
    inline bool applicable(void) const
    {
      
      if(state.pr_oppGoalKick)
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
