#pragma once
#include <utility>
#include "play.hpp"
#include "beliefState.h"
#include "tactic.h"

/* This play positions the bots in the position in case we lose the toss and the Opposition strikes the ball first*/
namespace Strategy
{
  class PPositionOppKickoff : public Play
  {
  public:
    inline PPositionOppKickoff(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOppKickoff";

      assert(HomeTeam::SIZE == 3); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-2*FREEKICK_RADIUS);
      param.PositionP.y = 0;
      param.PositionP.finalSlope=PI/2;
      roleList[0].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-HALF_FIELD_MAXX)/4;
      param.PositionP.y = HALF_FIELD_MAXY/2;
      param.PositionP.finalSlope  = 0;
      roleList[1].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-HALF_FIELD_MAXX)/4;
      param.PositionP.y = -HALF_FIELD_MAXY/2;
      param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
      // param.PositionP.align = false;
      // param.PositionP.x = ForwardX(-CENTER_CIRCLE_DIAMETER);
      // param.PositionP.y = 0;
      // param.PositionP.finalSlope  = 0;
      // roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
      //      /* Always make one robot as goalie*/
      // param.PositionP.align = false;
      // param.PositionP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.5);
      // param.PositionP.y = 0;
      // param.PositionP.finalSlope  =  PI/2;
      // roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }

    inline ~PPositionOppKickoff()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_oppKickOff)
        return true;
      return false;
    }

    inline Result done(void) const
    {
      
      return NOT_TERMINATED;
    }
  }; // class PPositionOppKickoff
} // namespace Strategy


