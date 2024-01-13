#pragma once
#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#define KICK_RADIUS 200
// this is when the foul was done by us and the ball is places in our side 
namespace Strategy
{
  class PPositionOppFreeBall : public Play
  {
		int getQuarter(int x, int y)
		{
			if (x >= 0 && y >= 0)
				return 1;
			else if( x <= 0 && y >= 0)
				return 2;
			else if( x <= 0 && y >= 0)
				return 4;
				else
				return 3 ;
		}
  public:
    inline PPositionOppFreeBall(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOppFreeBall";

      assert(HomeTeam::SIZE == 3); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;			
      
	  Vector2D<int> finalPoint ;
			/* Role 0 - for the goalie */
	  roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
	  
	//  int quadrant = getQuarter(state.ballPos.x,state.ballPos.y);
      /* Role 1 - for the defender 1*/
	  param.PositionP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*4);
	  param.PositionP.y = -1*SGN(state.ballPos.y)*BOT_RADIUS ;
	  param.PositionP.finalSlope = 0 ;
	  param.PositionP.align = true ;
	  roleList[1].push_back(std::make_pair(Tactic::Position,param));
	  
	  //Role 2
	  Vector2D<int> dpoint(state.ballPos.x - ForwardX(FREEBALL_RADIUS),state.ballPos.y);
	  finalPoint = position_correcter(&state,dpoint);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position,param));
	  
	  //Role 3
	//   Vector2D<int> dpoint1(state.ballPos.x,SGN(-state.ballPos.y)*HALF_FIELD_MAXY/2);
	//   finalPoint = position_correcter(&state,dpoint1);
	//   param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
    //   roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
	//   //Role 4
	//   Vector2D<int> dpoint2(state.ballPos.x - ForwardX(HALF_FIELD_MAXX)/4,-1*(state.ballPos.y)/2);
	//   finalPoint = position_correcter(&state,dpoint2);
	//   param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
    //   param.PositionP.finalSlope  = 0;
    //   roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
	
	virtual void reevaluateRoleParams()
    {
		{
			Tactic::Param &param = roleList[1][0].second;
			param.PositionP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*4);
	  param.PositionP.y = -1*SGN(state.ballPos.y)*BOT_RADIUS ;
	  param.PositionP.finalSlope = 0 ;
	  param.PositionP.align = true ;
		}
		{         Vector2D<int> finalPoint;
		         Tactic::Param &param = roleList[2][0].second;
                       Vector2D<int> dpoint(state.ballPos.x - ForwardX(FREEBALL_RADIUS),state.ballPos.y);
	  finalPoint = position_correcter(&state,dpoint);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = 0;
        }
        {
          			Vector2D<int> finalPoint;
						Tactic::Param &param = roleList[3][0].second;
                       Vector2D<int> dpoint1(state.ballPos.x,SGN(-state.ballPos.y)*HALF_FIELD_MAXY/2);
	  finalPoint = position_correcter(&state,dpoint1);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
        }
        {
			Vector2D<int> finalPoint;
            Tactic::Param &param = roleList[4][0].second;
                       Vector2D<int> dpoint2(state.ballPos.x - ForwardX(HALF_FIELD_MAXX)/4,-1*(state.ballPos.y)/2);
	  finalPoint = position_correcter(&state,dpoint2);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = 0;
        }
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }

    inline ~PPositionOppFreeBall()
    { }

    inline bool applicable(void) const
    {
      // No need for this play...
      return false;
    }

    inline Result done(void) const
    {
      // TODO make it more sophisticated and also use the timeout info to determine if the play has terminated
      // printf("Done condition not finalised\n");
      return NOT_TERMINATED;
    }
  }; // class PPositionOurFreeBall
} // namespace Strategy