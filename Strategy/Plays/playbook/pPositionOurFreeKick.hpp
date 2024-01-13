#ifndef PPOSITIONOURFREEKICK_HPP
#define PPOSITIONOURFREEKICK_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
#include <queue>
#include "playBook.h"

#define KICK_RADIUS 200
/* This play positions the bots in the position in case its our Free Kick*/
namespace Strategy
{

 class PPositionOurFreeKick : public Play 
  {
  public:
    inline PPositionOurFreeKick(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOurFreeKick";

      assert(HomeTeam::SIZE == 3);
      setTimeout(100, 1);
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      
	Vector2D<int> finalpoint ;
    		
			/* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));      //check for the rule of quadrant
      
      /* Role 1 - Defender 1*/
	  
	  	Vector2D<int> dpoint1(state.ballPos.x-ForwardX(FREEKICK_RADIUS),state.ballPos.y-SGN(state.ballPos.y)*2*BOT_RADIUS);     
 
     finalpoint = Play::position_our_bot(&state,dpoint1,FREEKICK_RADIUS);
	  param.PositionP.x = finalpoint.x;
	  param.PositionP.y = finalpoint.y;
	  param.PositionP.finalSlope  = Vector2D<int>::angle(finalpoint,state.ballPos);
	  roleList[1].push_back(std::make_pair(Tactic::Position,param));
      
	  
	  //Role 2 
	  
      Vector2D<int> dpoint(state.ballPos.x-1.5*ForwardX(BOT_RADIUS) ,state.ballPos.y+SGN(state.ballPos.y)*BOT_RADIUS);
      param.PositionP.align = false;
      finalpoint = Play::position_our_bot(&state,dpoint,FREEKICK_RADIUS);
	  param.PositionP.x = finalpoint.x;
	  param.PositionP.y = finalpoint.y;
      param.PositionP.finalSlope  = Vector2D<int>::angle(finalpoint,state.ballPos);   
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
  
	  // //Role 3
    //   finalpoint = Play::position_our_bot(&state,dpoint1,FREEKICK_RADIUS);
	  // param.PositionP.x = finalpoint.x;
	  // param.PositionP.y = finalpoint.y;
	  // param.PositionP.finalSlope  = Vector2D<int>::angle(finalpoint,state.ballPos);
    //   roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
	  // //Role 4 

    //   Vector2D<int> dpoint0(ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3),state.homePos[0].y - SGN(state.homePos[0].y)*1.5*BOT_RADIUS);
    //      	//  Client::debugClient->SendMessages("going");
	  //       finalpoint = Play::position_correcter(&state,dpoint0);
    //     	//finalpoint.x = dpoint0.x ; finalpoint.y = dpoint0.y ;
		// 	param.PositionP.x = finalpoint.x ; param.PositionP.y = finalpoint.y ;
	  //       param.PositionP.finalSlope  = 0;
    //   roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
    virtual void reevaluateRoleParams()
    {
		{  Vector2D<int> finalpoint;
		   Tactic::Param &param = roleList[1][0].second;
           Vector2D<int> dpoint(state.ballPos.x-ForwardX(BOT_RADIUS),state.ballPos.y+SGN(state.ballPos.y)*BOT_RADIUS);
           param.PositionP.align = false;
           finalpoint = Play::position_our_bot(&state,dpoint,FREEKICK_RADIUS);
	       param.PositionP.x = finalpoint.x;
		   param.PositionP.y = finalpoint.y;
			param.PositionP.finalSlope  = Vector2D<int>::angle(finalpoint,state.ballPos);
        }
		{   Tactic::Param &param = roleList[2][0].second;
		    Vector2D<int> finalpoint;
		    Vector2D<int> dpoint0(ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3),state.homePos[0].y - SGN(state.homePos[0].y)*1.5*BOT_RADIUS);
         	//  Client::debugClient->SendMessages("going");
	        finalpoint = Play::position_correcter(&state,dpoint0);
			param.PositionP.x = finalpoint.x ; param.PositionP.y = finalpoint.y ;
	        param.PositionP.finalSlope  = 0;
			roleList[1].push_back(std::make_pair(Tactic::Position,param));
		}
        {
            Vector2D<int> finalpoint;
			Tactic::Param &param = roleList[3][0].second;
            Vector2D<int> dpoint1(state.ballPos.x-ForwardX(FREEKICK_RADIUS),state.ballPos.y-SGN(state.ballPos.y)*2*BOT_RADIUS);     
            finalpoint = Play::position_our_bot(&state,dpoint1,FREEKICK_RADIUS);
	        param.PositionP.x = finalpoint.x;
	        param.PositionP.y = finalpoint.y;
	        param.PositionP.finalSlope  = Vector2D<int>::angle(finalpoint,state.ballPos);
        }
        {
             Vector2D<int> finalpoint;
			 Tactic::Param &param = roleList[4][0].second;
            Vector2D<int> dpoint2(ForwardX(HALF_FIELD_MAXX/4),SGN(-state.ballPos.y)*HALF_FIELD_MAXY/3); 
	         finalpoint =  Play::position_our_bot(&state,dpoint2,FREEKICK_RADIUS);
             param.PositionP.x = finalpoint.x;
	         param.PositionP.y = finalpoint.y;
             param.PositionP.finalSlope  = Vector2D<int>::angle(finalpoint,state.ballPos);
        }
        //printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }
    inline ~PPositionOurFreeKick()
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