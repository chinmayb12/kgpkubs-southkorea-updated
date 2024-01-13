#ifndef PPOSITIONOURGOALKICK_HPP
#define PPOSITIONOURGOALKICK_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#define KICK_RADIUS 200
/* This play positions the bots in the position in case its our Free Kick*/
namespace Strategy
{
  class PPositionOurGoalKick : public Play
  {
  public:
    inline PPositionOurGoalKick(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOurGoalKick";

      assert(HomeTeam::SIZE == 3);
      setTimeout(100, 1);
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      
	  Vector2D<int> finalPoint ;
	  /* for goalie */
	  roleList[0].push_back(std::make_pair(Tactic::GoalieOur, param));
      
      /* Role 1 - Defender 1*/
	  Vector2D<int> dpoint0((-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*5),-1*(OUR_GOAL_MAXY + BOT_RADIUS));
	  finalPoint = position_our_bot(&state,dpoint0,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	  param.PositionP.finalSlope = 0 ;
	  roleList[1].push_back(std::make_pair(Tactic::Position,param));
      
	  // Role 2
	  Vector2D<int> dpoint(ForwardX(-HALF_FIELD_MAXX/2),HALF_FIELD_MAXY/2);
	  finalPoint = position_our_bot(&state,dpoint,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
	  // //Role 3
	  // Vector2D<int> dpoint1(ForwardX(-CENTER_CIRCLE_DIAMETER/2),HALF_FIELD_MAXY/4);
	  // finalPoint = position_our_bot(&state,dpoint1,0);
	  // param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
    //   param.PositionP.finalSlope  = 0;
    //   roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
	  // //Role 4
	  // Vector2D<int> dpoint2(ForwardX(HALF_FIELD_MAXX/2.5),HALF_FIELD_MAXY/2);
	  // finalPoint = position_our_bot(&state,dpoint2,0);
	  // param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
    //   param.PositionP.finalSlope  = 0;
    //   roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }

	virtual void reevaluateRoleParams()
    {
        {
			Vector2D<int> finalPoint;
            Tactic::Param &param = roleList[1][0].second;
             Vector2D<int> dpoint0((-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*5),-1*(OUR_GOAL_MAXY + BOT_RADIUS));
	  finalPoint = position_our_bot(&state,dpoint0,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	  param.PositionP.finalSlope = 0 ;
        }

        {
			Vector2D<int> finalPoint;
            Tactic::Param &param = roleList[2][0].second;
             Vector2D<int> dpoint(ForwardX(-HALF_FIELD_MAXX/2),HALF_FIELD_MAXY/2);
	  finalPoint = position_our_bot(&state,dpoint,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = 0;
        }
        {   
			Vector2D<int> finalPoint;
            Tactic::Param &param = roleList[3][0].second;
           Vector2D<int> dpoint1(ForwardX(-CENTER_CIRCLE_DIAMETER/2),HALF_FIELD_MAXY/4);
	  finalPoint = position_our_bot(&state,dpoint1,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = 0;
			
        }
		{
			Vector2D<int> finalPoint;
		Tactic::Param &param = roleList[4][0].second;
		 Vector2D<int> dpoint2(ForwardX(HALF_FIELD_MAXX/2.5),HALF_FIELD_MAXY/2);
	  finalPoint = position_our_bot(&state,dpoint2,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = 0;
	 
		}
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }
	
    inline ~PPositionOurGoalKick()
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