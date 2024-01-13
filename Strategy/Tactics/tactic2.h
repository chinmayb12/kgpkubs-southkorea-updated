#ifndef TACTIC_NEW_H
#define TACTIC_NEW_H

#include <list>
#include "skillSet.h"
#include <cstring>
#include "comm.h"

namespace Strategy
{
  class BeliefState;
}

namespace Strategy {
	class Tactic2 : public Tactic {
	private:
		friend class PExec;
		void addsegment(std::vector<std::pair<float, float> >&, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&) const;
	public:
		const int firstBotID;
		const int secondBotID;
		SkillSet * skillSetFirst;
		SkillSet * skillSetSecond;
		inline Tactic2(ID tID, const BeliefState* state, int firstBot, int secondBot) : Tactic(tID, state, firstBot),
		  firstBotID(firstBot),
		  secondBotID(secondBot)
		{
		  skillSetFirst = skillSet;
		  skillSetSecond = new SkillSet(state, secondBotID);
		  tState = RUNNING;
		} // Tactic
		
		inline ~Tactic2()
		{
		  delete skillSetFirst;
		  delete skillSetSecond;
		} // ~Tacti
	};
}
#endif
