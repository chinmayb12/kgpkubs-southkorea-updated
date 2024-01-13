#include <iostream>
#include <cstring>
#include <ctime>
#include "tExec.h"
#include "beliefState.h"
#include "skillSet.h"
#include "tBackup.hpp"
#include "tinclude.h"
using namespace std;
using namespace Util;
using namespace HAL;

namespace Strategy
{
  TExec::TExec(BeliefState* state) :
    tIDSh(Tactic::Stop),
    tStateSh(Tactic::INIT)
  {
    // Resetting the tactic list
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
		for(int botID = 0; botID < HomeTeam::SIZE;botID++)
		{
			tacticList[tID][botID] = NULL;
		}
    }

    // List all Tactic ID -> Tactic object pairs here for each robot
	for(int botID=0; botID < HomeTeam::SIZE; botID++)
	{
		tacticList[Tactic::Block][botID]               = new TBlock(state, botID);
		tacticList[Tactic::ChargeBall][botID]          = new TCharge(state, botID);
		tacticList[Tactic::CoverGoal][botID]           = new TCoverGoal(state,botID);
		tacticList[Tactic::DefendLine][botID]          = new TDefendLine(state, botID);
		tacticList[Tactic::DefendPoint][botID]         = new TDefendPoint(state, botID);
		tacticList[Tactic::GoalieOur][botID]           = new TGoalKeepingOurSide(state, botID);
		tacticList[Tactic::Pass][botID]                = new TPass(state, botID);
		tacticList[Tactic::Kickoff][botID]         = new TKickoff(state, botID);
		tacticList[Tactic::Position][botID]            = new TPosition(state, botID);
		tacticList[Tactic::Defend][botID]              = new TDefend(state,botID);
		tacticList[Tactic::Attack][botID]              = new TAttack(state,botID);
		tacticList[Tactic::SupportAttack2015][botID]	= new TSupportAttack2015(state,botID);
		tacticList[Tactic::Stop][botID]                = new TStop(state, botID);
		tacticList[Tactic::Velocity][botID]            = new TVelocity(state, botID);
		tacticList[Tactic::Backup][botID]              = new TBackup(state, botID);
		tacticList[Tactic::ReceiveBall][botID]         = new TReceiveBall(state, botID);
		tacticList[Tactic::TestgotoPoint][botID] 			= new Testgotopoint(state,botID);
		tacticList[Tactic::TestbotRace][botID] 				= new TestbotRace(state,botID);
		tacticList[Tactic::GoalKick][botID]						= new TGoalKick(state,botID);
		tacticList[Tactic::Shoot][botID]                  = new TShoot(state,botID);
		tacticList[Tactic::Defender2][botID]		   = new TDefender2(state, botID);
		tacticList[Tactic::Goalie2][botID]		   = new TGoalie2(state, botID);	
		tacticList[Tactic::DWDefender2015][botID]		   = new TDWDefender2015(state, botID);
		tacticList[Tactic::CoverGoal2015][botID]           = new TCoverGoal2015(state,botID);
		tacticList[Tactic::CoverGoalPairLeft][botID]           = new TCoverGoalPairLeft(state,botID);
		tacticList[Tactic::CoverGoalPairRight][botID]           = new TCoverGoalPairRight(state,botID);
		tacticList[Tactic::ReceiveBall2015][botID]         = new TReceiveBall2015(state, botID);
		tacticList[Tactic::ReceiveSpline][botID]         = new TReceiveSpline(state, botID);
		tacticList[Tactic::AttackSpline][botID]			   = new TAttackSpline(state, botID);
		tacticList[Tactic::Attack2015][botID]			   = new TAttack2015(state, botID);
//		tacticList[Tactic::AttackDuo][botID]			   = new TAttackDuo(state, botID);
		//tacticList[Tactic::Clear]								= new TClear(state,botID);
		//tacticList[Tactic::AttackDuo] = new TAttackDuo() -- how the fuck am i supposed to add two bot ids ??? :-(
	}
    // Initialization check
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
		for(int botID = 0; botID < HomeTeam::SIZE; botID++)
		{
		assert(tacticList[tID][botID] != NULL); // Tactic enum not mapped to the corresponding Tactic object
		}
	}
  } // Robot

  TExec::~TExec()
  {    
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
		for(int botID = 0; botID < HomeTeam::SIZE; botID++)
		{
			if (tacticList[tID] != NULL)
			{
				delete tacticList[tID][botID];
			}
		}
	}
  } // ~Robot

  void TExec::execute(BeliefState *state, Tactic::ID tID, Tactic::Param tParam,int botID)
  {
    //Tactic::ID    tID = Tactic::Stop;
    //Tactic::Param tParam;
       // tacticList[tID]->initialize();
	   
		cout << "\n\n" << tID << "\n\n" << botID << "\n\n";
        tacticList[tID][botID]->execute(tParam);
	   
  } // run
} // namespace Strategy
