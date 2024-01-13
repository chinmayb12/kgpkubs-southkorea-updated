#ifndef TTATTACKDUO_HPP
#define TTATTACKDUO_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "tactic2.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "intersection.hpp"
#define ANGLE_TO_DIST 0
namespace Strategy
{
  class TAttackDuo : public Tactic2
  {
    
  public:
    TAttackDuo(const BeliefState* state, int firstBot, int secondBot) :
      Tactic2(Tactic::AttackDuo, state, firstBot,secondBot)
    {
      
    } // TAttackDuo

    ~TAttackDuo()
    { } // ~TAttack
   
    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      return 0;

    } // chooseBestBot

   
    void execute(const Param& tParam)
    {
			printf("Being called from tAttackDuo -> execute :D \n");
      sID = SkillSet::Spin;
			sParam.SpinP.radPerSec = MAX_BOT_OMEGA * (state->homePos[botID].y > 0? ForwardX(1): ForwardX(-1));
			skillSetFirst->executeSkill(sID, sParam);           
			sParam.SpinP.radPerSec =  - sParam.SpinP.radPerSec;
			skillSetSecond->executeSkill(sID,sParam);
    }
		
		
		
		
  }; // class TAttackDuo
} // namespace Strategy

#endif // TTAttackDuo_HPP