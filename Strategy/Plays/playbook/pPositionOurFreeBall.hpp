#pragma once
#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#define KICK_RADIUS 200
// this is when the foul was done by the opponent and the ball is places on the opponents side 
namespace Strategy
{
  class PPositionOurFreeBall : public Play
  {
		int getQuarter(int x, int y)
		{
			if (x <= 0 && y >= 0)
				return 0;
			else if( x <= 0 && y <= 0)
				return 1;
			else if( x >= 0 && y <= 0)
				return 2;
		}
  public:
    inline PPositionOurFreeBall(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOurFreeBall";

      assert(HomeTeam::SIZE == 3); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;			
      Vector2D<int> finalPoint ;
			/* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
	  
      /* Role 1 - shoot position 1*/
	  Vector2D<int> dpoint(state.ballPos.x - ForwardX(FREEBALL_RADIUS),state.ballPos.y +  SGN(state.ballPos.y)*2*BOT_RADIUS);
	  finalPoint = position_our_bot(&state,dpoint,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
      roleList[1].push_back(std::make_pair(Tactic::Position,param));
	  //
	  
	//Role 2  
      param.PositionP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*6); //check the position  ...for the quater
	   if(SGN(state.ballPos.x)>0)
	    param.PositionP.y = -SGN(state.ballPos.y)*2*BOT_RADIUS;
	   else
	   param.PositionP.y = SGN(state.ballPos.y)*2*BOT_RADIUS ;
	  
	  param.PositionP.finalSlope = 0 ;
	  param.PositionP.align = true ;
	  roleList[2].push_back(std::make_pair(Tactic::Position,param));
	  
	  // //Role 3
	  // Vector2D<int> dpoint1(state.ballPos.x,SGN(-state.ballPos.y)*HALF_FIELD_MAXY/2);
	  // finalPoint = position_our_bot(&state,dpoint1,0);
	  // param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
    //   param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
    //   roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
    //   //Role 4
	  // Vector2D<int> dpoint2(SGN(-1*state.ballPos.x)*(HALF_FIELD_MAXX/2),-1*(state.ballPos.y));
	  // finalPoint = position_our_bot(&state,dpoint2,0);
	  // param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
    //   param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
    //   roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
	
	virtual void reevaluateRoleParams()
    {
		{    Vector2D<int> finalPoint; 
			Tactic::Param &param = roleList[1][0].second ;
			Vector2D<int> dpoint(state.ballPos.x - ForwardX(FREEBALL_RADIUS),state.ballPos.y +  SGN(state.ballPos.y)*2*BOT_RADIUS);
	    finalPoint = position_our_bot(&state,dpoint,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);	 
		}
		{  Vector2D<int> finalPoint;
            Tactic::Param &param = roleList[2][0].second;
                        param.PositionP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*6); //check the position  ...for the quater
	   if(SGN(state.ballPos.x)>0)
	    param.PositionP.y = -SGN(state.ballPos.y)*2*BOT_RADIUS;
	   else
	   param.PositionP.y = SGN(state.ballPos.y)*2*BOT_RADIUS ;
	  
	  param.PositionP.finalSlope = 0 ;
	  param.PositionP.align = true ;
        }
        {  Vector2D<int> finalPoint;
            Tactic::Param &param = roleList[3][0].second;
                        Vector2D<int> dpoint1(state.ballPos.x,SGN(-state.ballPos.y)*HALF_FIELD_MAXY/2);
	  finalPoint = position_our_bot(&state,dpoint1,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
        }
		{ Vector2D<int> finalPoint;
            Tactic::Param &param = roleList[4][0].second;
                        Vector2D<int> dpoint2(SGN(-1*state.ballPos.x)*(HALF_FIELD_MAXX/2),-1*(state.ballPos.y));
	  finalPoint = position_our_bot(&state,dpoint2,0);
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
        }
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }

    inline ~PPositionOurFreeBall()
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