#ifndef PPOSITIONOPPFREEKICK_HPP
#define PPOSITIONOPPFREEKICK_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#define KICK_RADIUS 200
/* This play positions the bots in the position in case we Defend a Free Kick*/
namespace Strategy
{
  class PPositionOppFreeKick : public Play
  {
  public:
    inline PPositionOppFreeKick(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOppFreeKick";

      assert(HomeTeam::SIZE == 3); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
	Vector2D<int> finalPoint;
    //  roleList[1].push_back(std::make_pair(Tactic::Position, param));
			
			/* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - Defender 1*/
      Vector2D<int> dpoint0(ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3),state.homePos[0].y - SGN(state.homePos[0].y)*2*BOT_RADIUS);
         	//  Client::debugClient->SendMessages("going");
	        finalPoint = position_correcter(&state,dpoint0);
        	//finalPoint.x = dpoint0.x ; finalPoint.y = dpoint0.y ;
			param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	        param.PositionP.finalSlope  = 0;
			roleList[1].push_back(std::make_pair(Tactic::Position,param));
      
	  //Role 2 
	  Vector2D<int> dpoint(state.ballPos.x,-SGN(state.ballPos.y)*HALF_FIELD_MAXX/4);
	//  Client::debugClient->SendMessages("going");
	 finalPoint = position_correcter(&state,dpoint);
	 finalPoint.x = dpoint.x ; finalPoint.y = dpoint.y ;

	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	  param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
	  // //Role 3
	  // Vector2D<int> dpoint1(state.ballPos.x-ForwardX(FREEKICK_RADIUS),state.ballPos.y);
	  // //finalPoint = position_correcter(&state,dpoint1);
	  // //param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
    //   Vector2D<int>Goal(ForwardX(HALF_FIELD_MAXX),0);
    //   param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
    //   roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
	  // //Role 4
	  // Vector2D<int> dpoint2(state.ballPos.x-ForwardX(HALF_FIELD_MAXX)/4,(state.ballPos.y)-SGN(state.ballPos.y)*3*BOT_RADIUS);
	  // //finalPoint = position_correcter(&state,dpoint2);
	  // finalPoint = dpoint2;
	  // param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	  // param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
    //   roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
	
	virtual void reevaluateRoleParams()
    {
        {
            Tactic::Param &param = roleList[1][0].second;
			Vector2D<int> finalPoint , dpoint(ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3),state.homePos[0].y - SGN(state.homePos[0].y)*2*BOT_RADIUS);
         	//  Client::debugClient->SendMessages("going");
	        finalPoint = position_correcter(&state,dpoint);
        	//finalPoint.x = dpoint.x ; finalPoint.y = dpoint.y ;
			param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	        param.PositionP.finalSlope  = 0;
        }
        {
            Tactic::Param &param = roleList[2][0].second;
            Vector2D<int> finalPoint , dpoint(state.ballPos.x,-SGN(state.ballPos.y)*HALF_FIELD_MAXX/4);
         	//  Client::debugClient->SendMessages("going");
	        finalPoint = position_correcter(&state,dpoint);
        	finalPoint.x = dpoint.x ; finalPoint.y = dpoint.y ;
			param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	        param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
        }
        {
            Tactic::Param &param = roleList[3][0].second;
            Vector2D<int> finalPoint ,dpoint1(state.ballPos.x-ForwardX(FREEKICK_RADIUS),state.ballPos.y);
	        finalPoint = position_correcter(&state,dpoint1);
	        param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
          //  Vector2D<int>Goal(ForwardX(HALF_FIELD_MAXX),0);
            param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
      
        }
        {
            Tactic::Param &param = roleList[4][0].second;
            Vector2D<int> finalPoint; 
			Vector2D<int> dpoint2(state.ballPos.x-ForwardX(HALF_FIELD_MAXX)/4,(state.ballPos.y)-SGN(state.ballPos.y)*3*BOT_RADIUS);
	        finalPoint = position_correcter(&state,dpoint2);
	        finalPoint = dpoint2;
	        param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	        param.PositionP.finalSlope  = Vector2D<int>::angle(finalPoint,state.ballPos);
			
        }
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }
	
    inline ~PPositionOppFreeKick()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_oppFreeKick)
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

#endif // PPOSITIONOPPFREEKICK