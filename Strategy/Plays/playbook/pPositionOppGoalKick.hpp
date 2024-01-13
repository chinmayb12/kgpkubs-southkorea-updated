#ifndef PPOSITIONOPPGOALKICK_HPP
#define PPOSITIONOPPGOALKICK_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#define KICK_RADIUS 200
/* This play positions the bots in the position in case its our Free Kick*/
namespace Strategy
{
   class PPositionOppGoalKick : public Play
  {
  public:
    inline PPositionOppGoalKick(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOppGoalKick";

      assert(HomeTeam::SIZE == 3);
      setTimeout(100, 1);
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      
			
			/* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - Defender 1*/
      roleList[1].push_back(std::make_pair(Tactic::CoverGoal,param));
	  
	  //Role 2
      
      param.PositionP.x =ForwardX(-BOT_RADIUS);
      param.PositionP.y = 0;
      param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
	  // //Role 3
	  
    //   param.PositionP.x = ForwardX(-CENTER_CIRCLE_DIAMETER/2);
    //   param.PositionP.y = HALF_FIELD_MAXY/2;
    //   param.PositionP.finalSlope  = 0;
    //   roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
	  // //Role 4
    //   param.PositionP.x = ForwardX(-CENTER_CIRCLE_DIAMETER/2);;
    //   param.PositionP.y = -HALF_FIELD_MAXY/2;
    //   param.PositionP.finalSlope  = 0;
    //   roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
    
	virtual void reevaluateRoleParams()
    {
        
        {
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[2][0].second;
             param.PositionP.x =ForwardX(-BOT_RADIUS);
      param.PositionP.y = 0;
      param.PositionP.finalSlope  = 0;
        }
        {   
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[3][0].second;
          param.PositionP.x = ForwardX(-CENTER_CIRCLE_DIAMETER/2);
      param.PositionP.y = HALF_FIELD_MAXY/2;
      param.PositionP.finalSlope  = 0;
			
        }
		{
			Vector2D<int> finalpoint;
		Tactic::Param &param = roleList[4][0].second;
		param.PositionP.x = ForwardX(-CENTER_CIRCLE_DIAMETER/2);;
      param.PositionP.y = -HALF_FIELD_MAXY/2;
      param.PositionP.finalSlope  = 0;
	 
		}
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }

    inline ~PPositionOppGoalKick()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_ourFreeKick)
        return true;
      return false;
    }

    inline Result done(void) const
    {
      // TODO make it more sophisticated and also use the timeout info to determine if the play has terminated
      // printf("Done condition not finalised\n");
      return NOT_TERMINATED;
    }
  }; // class PTestPlay
} // namespace Strategy

#endif // PPOSITIONOURFREEKICK