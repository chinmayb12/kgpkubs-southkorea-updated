/* This file contains the PS class(Play Selector) that implements the algorithm
 * to select the play to be implemented in each frame. It uses the probabilistic model 
 * to select the most appropriate play among the list of applicable plays.
 */

#include <cstdlib>
#include "ps.h"
#include "playBook.h"
#include "play.hpp"

using namespace std;
#define D_PS 1
namespace Strategy
{
  PS::PS(const BeliefState* state) : 
    PlayBook(state),
    playID(PlayBook::None)
  {
    for (int playID=0; playID<PlayBook::MAX_PLAYS; ++playID)
    {
      appl[playID] = false;
      pProb[playID] = 0.0f;
    }
 }

  PS::~PS()
  { }

	void PS::select(int *playid)
  {
    // Find the applicable plays
    const int num_plays = PlayBook::MAX_PLAYS;
    debug(D_PS, "Selecting Next Play...");
    debug(D_PS, "Game Running: ", state->pr_gameRunning);
    debug(D_PS, "Ball Our:", state->pr_ourBall, " Opp:", state->pr_oppBall, " Loose:", state->pr_looseBall);
    printf("Game Running: %d",state->pr_gameRunning);
    for (int pID = 0; pID < num_plays; ++pID)
    {
      appl[pID] = playList[pID]->applicable();
      debug(D_PS, "Play:", playList[pID]->name, 
            "Applicability:", appl[pID], "Weight:", playList[pID]->weight);
    }

    float cumWeight = 0.0f;
    for (int pID = 0; pID < num_plays; ++pID)
    {
      if (appl[pID])
      {
        cumWeight += playList[pID]->weight;
      }
    }

    for (int pID = 0; pID < num_plays; ++pID)
    {
      if (appl[pID])
      {
        pProb[pID] = playList[pID]->weight/float(cumWeight);
      }
      else
      {
        pProb[pID] = 0.0f;
      }
    }

    float cumProb = 0.0f;
    float randVal = (float)std::rand()/RAND_MAX + 0.000001f;
    if (randVal >= 1.0f)
    {
      randVal = 0.999999f;
    }

     // playID = PlayBook::PenaltyOur ;
     //playID = PlayBook::PositionOurPenalty;
	 //playID = PlayBook::PositionGather;
    playID = PlayBook::Offense1;
	  //playID = PlayBook::OurFreeKick;
    //playID = PlayBook::PenaltyOur;
	 //playID = PlayBook::Kickoff ;
	//playID = PlayBook::PositionOurKickoff;
	
	//printf("pop called");
	//playID = PlayBook::Offense1;
/*
    for (int pID = 0; pID < num_plays; ++pID)
    {
      cumProb += pProb[pID];
      debug(1,"CumProb : ",cumProb,"pID : ",pID,"randVal : ",randVal);
      if (cumProb >= randVal && appl[pID])
      {
        playID = (PlayID)pID;
        break;
      }
    }
*/

		/*Arpit: treating special case for our penalty kcik and kickoff*/
	/*if(state->pr_ourKickOffStart) {
			playID = PlayBook::Kickoff;
		} else if(state->pr_ourPenaltyKickStart) {
			playID = PlayBook::PenaltyOur;
		}
	if (playID == PlayBook::None)
		playID = PlayBook::Offense1;*/
		
		//cout << "yes";
    assert(playID != PlayBook::None); // No play selected

    playList[playID]->startTimer();
	if(playid!=NULL)*playid = playID;
	
  } // select
  
  void PS::selectfromStr_Gui(int pID){
	  playID = (Strategy::PlayBook::PlayID)pID;
	  playList[playID]->startTimer();
  }
} // namespace Strategy