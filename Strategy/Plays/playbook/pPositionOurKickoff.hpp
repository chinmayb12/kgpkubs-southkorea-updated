#pragma once

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"

/* This play positions the bots in the position in case its our Free Kick*/
namespace Strategy
{
  class PPositionOurKickoff : public Play
  {
  public:
    inline PPositionOurKickoff(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOurKickoff";

      assert(HomeTeam::SIZE == 3);
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;     
			
			
			/* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - Defender 1*/
      roleList[1].push_back(std::make_pair(Tactic::CoverGoal,param));
      
			/* Role 2 - Passer. Tries to be in a position to pass to Role 3 at start of game. */
      param.PositionP.align = true;
			float theta = 60 *PI/180.0;
      param.PositionP.x = ForwardX(KICKOFF_RADIUS*cos(theta));
      param.PositionP.y = KICKOFF_RADIUS*sin(theta);			
      param.PositionP.finalSlope  = 2*PI + ForwardX(theta);
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
// 			/* Role 3 - Receiver. Tries to be in a position to receive the pass. */
//       param.PositionP.x = -ForwardX(KICKOFF_RADIUS*5*cos(theta));
//       param.PositionP.y =  -KICKOFF_RADIUS*2.5*sin(theta);
//       param.PositionP.finalSlope  = 0;
//       roleList[3].push_back(std::make_pair(Tactic::Position, param));
		
// /*		param.SplineGoToPointP.x = -ForwardX(KICKOFF_RADIUS*5*cos(theta));
// 		param.SplineGoToPointP.y = -KICKOFF_RADIUS*2.5*sin(theta);
// 		param.GoToPointP.finalSlope  = 0;
// 		param.SplineGoToPointP.finalVelocity = 0;
// 		rolelist[3].push_back(std::make_pair(Tactic::SplineGoToPoint, param));*/
		
// 			/* Role 4 - vela. */
//       param.PositionP.x = -ForwardX(HALF_FIELD_MAXX/3);
//       param.PositionP.y = 0;
//       param.PositionP.finalSlope  = 0;
//       roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
    
    inline ~PPositionOurKickoff()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_ourKickOff)
        return true;
      return false;
    }

    inline Result done(void) const
    {
      // TODO make it more sophisticated and also use the timeout info to determine if the play has terminated
      // printf("Done condition not finalised\n");
      return NOT_TERMINATED;
    }
  }; // class PPositionOurKickoff
} // namespace Strategy
