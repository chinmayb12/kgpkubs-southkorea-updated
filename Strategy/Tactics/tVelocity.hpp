#ifndef TVELOCITY_HPP
#define TVELOCITY_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include <fstream>
#include <sys/time.h>

namespace Strategy
{
  class TVelocity : public Tactic
  {
  public:
    TVelocity(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TVelocity

    ~TVelocity()
    { } // ~TVelocity

    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        return *it;
      }
      Util::Logger::abort("No bot available for role assignment");
      return -1;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      sleep(10000);
		//for logging ball position
		FILE *out = fopen("/home/robocup/botVelocity.txt", "a");
		struct timeval nowTime;
		gettimeofday(&nowTime, NULL);
		 double elapsedMs = (nowTime.tv_sec*1000.0+nowTime.tv_usec/1000.0);
		fprintf(out, "%f %f \n", state->homeVlVr[botID].x, state->homeVlVr[botID].y);
	//	out << state->homeVlVr[botID].x << " " << state->homeVlVr[botID].y  << " " << elapsedMs<< endl;
//		out.close();
		fclose(out);
      printf("Velocity BotID: %d\n",botID);
	  printf("Left Right %d %d\n", tParam.VelocityP.vl, tParam.VelocityP.vr);
      // Select the skill to the executed next
      sID = SkillSet::Velocity;
      sParam.VelocityP.vl = tParam.VelocityP.vl;
      sParam.VelocityP.vr = tParam.VelocityP.vr;      
      // Execute the selected skill
      skillSet->executeSkill(sID, sParam);

      if (state->homeVel[botID].absSq() > ZERO_VELOCITY_THRESHOLD * ZERO_VELOCITY_THRESHOLD)
      {
        tState = COMPLETED;
      }
    }
  }; // class TVelocity
} // namespace Strategy

#endif // TVELOCITY_HPP
