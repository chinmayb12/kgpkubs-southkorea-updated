#ifndef PLAY_BOOK_H
#define PLAY_BOOK_H
#include <utility>
//
#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
//
// Forward Declarations
namespace Strategy
{
  class BeliefState;
  class Play;
}

namespace Strategy
{
  class PlayBook
  {
  public:
    enum PlayID
    {
      TestPlay, //0
      SetPosition,
			PenaltyOur,
			PenaltyOpp,
      Stop,      
      OurGoalKick,
      OppGoalKick,//6
      SuperOffense,
      Offense1,
      Offense2,//9
      SuperDefense,
      Defense1,
      Defense2,//12
			//Arpit plays:
		PositionOurKickoff, //sorry, i didnt see PositionOurStrike already did the same thing... so i removed it.
		PositionOppKickoff,
		Kickoff,
		PositionOurPenalty,
	    TakeGoalKick,
		PositionOurFreeKick,
		PositionOppFreeKick,
		PositionOurFreeBall,
		PositionOppFreeBall,
		PositionOppPenalty,
		PositionOurGoalKick,
		PositionOppGoalKick,
	    PositionGather,
		OppFreeBall,
		OurFreeBall,
		OppFreeKick,
		OurFreeKick,
      None,
      MAX_PLAYS //16
    };

  protected:
    const BeliefState* state;
    Play*              playList[MAX_PLAYS];

  public:
    PlayBook(const BeliefState* state);
    
    
    ~PlayBook();

    void reload(void);
  }; // class PlayBook
} // namespace Strategy

#endif // PLAY_BOOK_H