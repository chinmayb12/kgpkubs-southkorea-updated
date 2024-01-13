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
  class PPositionOurPenalty : public Play
  {
    public: 
    inline PPositionOurPenalty(const BeliefState& state) : Play(state)
    {
		
		printf("Posiition our penalty is being executed");
      name = "PositionOurPenalty";
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      assert(HomeTeam::SIZE ==3);
      Tactic::Param param;
      param.PositionP.align = -PI;
	  
	  Vector2D<int> finalpoint ;
      /* Role 0 - Goalie*/
      
	 roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
	  
  /* Role 1 - Position 1*/    //shooter
	 param.PositionP.x = state.ballPos.x - ForwardX(3*BOT_RADIUS);// state.ballPos.x - 2*BOT_RADIUS ;
	 param.PositionP.y = state.ballPos.y;
	 param.PositionP.finalSlope = 0;
      roleList[1].push_back(std::make_pair(Tactic::Position,param));
 /* Role 1 - Position 2*/
      Vector2D<int> dpoint1(-ForwardX(2*BOT_RADIUS),0);     
	  finalpoint = position_our_bot(&state,dpoint1,0);
	  param.PositionP.x = finalpoint.x;
	  param.PositionP.y = finalpoint.y;
	  param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position,param));
      
//  /* Role 1 - Position 3*/   //shooter
// Vector2D<int> dpoint(-ForwardX(2*BOT_RADIUS),HALF_FIELD_MAXY/2);     
// 	  finalpoint = position_our_bot(&state,dpoint,0);
// 	  param.PositionP.x = finalpoint.x;
// 	  param.PositionP.y = finalpoint.y;
// 	  param.PositionP.finalSlope  = 0;
//       roleList[3].push_back(std::make_pair(Tactic::Position,param));
	  
//  /* Role 1 - Position 4*/
//       Vector2D<int> dpoint3(-ForwardX(2*BOT_RADIUS),-HALF_FIELD_MAXY/2);     
// 	  finalpoint = position_our_bot(&state,dpoint3,0);
// 	  param.PositionP.x = finalpoint.x;
// 	  param.PositionP.y = finalpoint.y;
// 	  param.PositionP.finalSlope  = 0;
//       roleList[4].push_back(std::make_pair(Tactic::Position,param));
      
      computeMaxTacticTransits();
    }
		
	virtual void reevaluateRoleParams()
    {
		{   
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[1][0].second;
            param.PositionP.x = state.ballPos.x - ForwardX(3*BOT_RADIUS);// state.ballPos.x - 2*BOT_RADIUS ;
	        param.PositionP.y = state.ballPos.y;
			param.PositionP.align = true;
	        param.PositionP.finalSlope = 0;
			
        }
        {
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[2][0].second;
             Vector2D<int> dpoint1(-ForwardX(2*BOT_RADIUS),0);     
	         finalpoint = position_our_bot(&state,dpoint1,0);
	        param.PositionP.x = finalpoint.x;
	        param.PositionP.y = finalpoint.y;
	        param.PositionP.finalSlope  = 0;
      
        }
        {   
		{  
			printf("pop");
			Vector2D<int> finalpoint;
            Tactic::Param &param = roleList[3][0].second;
            Vector2D<int> dpoint(-ForwardX(2*BOT_RADIUS),HALF_FIELD_MAXY/2);     
	        finalpoint = position_our_bot(&state,dpoint,0);
	        param.PositionP.x = finalpoint.x;
	        param.PositionP.y = finalpoint.y; 
	        param.PositionP.finalSlope  = 0;
        }
			
        }
		{
		Vector2D<int> finalpoint;
	Tactic::Param &param = roleList[4][0].second;
		Vector2D<int> dpoint3(-ForwardX(2*BOT_RADIUS),-HALF_FIELD_MAXY/2);     
	  finalpoint = position_our_bot(&state,dpoint3,0);
	  param.PositionP.x = finalpoint.x;
	  param.PositionP.y = finalpoint.y;
	  param.PositionP.finalSlope  = 0;
		
		}
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }
    inline ~PPositionOurPenalty()
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