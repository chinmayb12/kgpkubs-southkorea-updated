#ifndef TEXEC_H
#define TEXEC_H

#include "tactic.h"
#include "tactic2.h"

// Forward Declarations
namespace Strategy
{
  class BeliefState;
}

namespace Strategy
{
  class TExec
  {
    //friend class PExec;

  public:
    // Stores the current tactic and is controlled by the Play Layer
    Tactic::ID     tIDSh;
    // Stores the current tactic's parameters and is controlled by the Play Layer
    Tactic::Param  tParamSh;
    // Stores the current tactic's state and is controlled by the Play Layer
    Tactic::TState tStateSh;
    // List of tactics: A map of ID -> Tactic objects
    Tactic* tacticList[Tactic::MAX_TACTICS][HomeTeam::SIZE];
    
  private:
    int botID;
	int	tID;

    // Belief State object that the robot will use
    //BeliefState*   state;
    // Shared Belief State object between it and the main thread
    //BeliefState&   stateSh;
    // The function in which the Tactic loop runs forever

  public:
    TExec(BeliefState* state);

    ~TExec();
	
	void execute(BeliefState* state,Tactic::ID tID,Tactic::Param tParam,int botID);
  }; // class Robot
} // namespace Strategy

#endif // ROBOT_H
