#ifndef PPOSITIONGATHER_HPP
#define PPOSITIONGATHER_HPP

#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"
// This play positions the bots in the position in case its our Free Kick
namespace Strategy
{
   class PPositionGather : public Play
  {
  public:
    inline PPositionGather(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionGather";

      assert(HomeTeam::SIZE == 3);
      setTimeout(100, 1);
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      
	  //Role 0 
	  param.PositionP.x = SGN(state.homePos[0].y)*HALF_FIELD_MAXX*0.7;
	  param.PositionP.y = HALF_FIELD_MAXY*0.8 ;
	  roleList[0].push_back(std::make_pair(Tactic::Position, param));
	  
	  //Role 1 
	  param.PositionP.x = SGN(state.homePos[0].x)*HALF_FIELD_MAXX*0.7;
	  param.PositionP.y = HALF_FIELD_MAXY*0.8  - 2*BOT_RADIUS ;
	  roleList[1].push_back(std::make_pair(Tactic::Position, param));
	  //Role 2
	  param.PositionP.x = SGN(state.homePos[0].x)*HALF_FIELD_MAXX*0.7;
	  param.PositionP.y = HALF_FIELD_MAXY*0.8 - 4*BOT_RADIUS ;
	  roleList[2].push_back(std::make_pair(Tactic::Position, param));
	  // //Role 3 
	  
	  // param.PositionP.x = SGN(state.homePos[0].x)*(HALF_FIELD_MAXX*0.7 - 4*BOT_RADIUS);
	  // param.PositionP.y = HALF_FIELD_MAXY*0.8 - 2*BOT_RADIUS ;
	  // roleList[3].push_back(std::make_pair(Tactic::Position, param));
	  // //Role 4 
	  
	  // param.PositionP.x = SGN(state.homePos[0].x)*(HALF_FIELD_MAXX*0.7 - 4*BOT_RADIUS);
	  // param.PositionP.y = HALF_FIELD_MAXY*0.8  - 4*BOT_RADIUS ;
	  // roleList[4].push_back(std::make_pair(Tactic::Position, param));
	  
      computeMaxTacticTransits();
    }
    
	virtual void reevaluateRoleParams()
    {
        {
		   Tactic::Param &param = roleList[0][0].second;
		   param.PositionP.x = SGN(state.homePos[0].x)*HALF_FIELD_MAXX*0.7;
	       param.PositionP.y = HALF_FIELD_MAXY*0.8 ;
	    }
		{
		   Tactic::Param &param = roleList[1][0].second;
		   param.PositionP.x = SGN(state.homePos[0].x)*HALF_FIELD_MAXX*0.7;
		   param.PositionP.y = HALF_FIELD_MAXY*0.8 - 2*BOT_RADIUS ;
	    }
        {
             Tactic::Param &param = roleList[2][0].second;
             param.PositionP.x = SGN(state.homePos[0].x)*HALF_FIELD_MAXX*0.7;
	         param.PositionP.y = HALF_FIELD_MAXY*0.8  - 4*BOT_RADIUS ;
        }
        {   
            Tactic::Param &param = roleList[3][0].second;
            param.PositionP.x = SGN(state.homePos[0].x)*(HALF_FIELD_MAXX*0.7 - 4*BOT_RADIUS);
	        param.PositionP.y = HALF_FIELD_MAXY*0.8 - 2*BOT_RADIUS ;
			
        }
		{
			Vector2D<int> finalpoint;
		    Tactic::Param &param = roleList[4][0].second;
		    param.PositionP.x = SGN(state.homePos[0].x)*(HALF_FIELD_MAXX*0.7 - 4*BOT_RADIUS);
	        param.PositionP.y = HALF_FIELD_MAXY*0.8  - 4*BOT_RADIUS ;
	 
		}
        printf("reevaling params in PositionGather.ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }

    inline ~PPositionGather()
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
