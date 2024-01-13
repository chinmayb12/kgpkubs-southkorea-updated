#include <iostream>
#include <cstring>
#include <ctime>
#include "robot.h"
#include "beliefState.h"
#include "skillSet.h"
#include "tBackup.hpp"
#include "tinclude.h"
using namespace std;
using namespace Util;
using namespace HAL;

namespace Strategy
{
  
  Util::CS*  Robot::reader_mutex = new Util::CS();
  Util::CS*  Robot::writer_mutex = 0;
  Util::CS*  Robot::writer_preference = 0;
  int        Robot::reader_count = 0;
  Robot::Robot(BeliefState& stateSh,
               CS*          tacticCS,
               int          botID) :
    tIDSh(Tactic::Stop),
    tStateSh(Tactic::INIT),
    botID(botID),
    tacticCS(tacticCS),
    stateSh(stateSh)
  {
    state = new BeliefState();  // Initializing with default constructor to avoid re-instantaition of referee box and vision thread
    
    // Resetting the tactic list
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
      tacticList[tID] = NULL;
    }

    // List all Tactic ID -> Tactic object pairs here for each robot
	tacticList[Tactic::Block]               = new TBlock(state, botID);
		tacticList[Tactic::ChargeBall]          = new TCharge(state, botID);
		tacticList[Tactic::CoverGoal]           = new TCoverGoal(state,botID);
		tacticList[Tactic::DefendLine]          = new TDefendLine(state, botID);
		tacticList[Tactic::DefendPoint]         = new TDefendPoint(state, botID);
		tacticList[Tactic::GoalieOur]           = new TGoalKeepingOurSide(state, botID);
		tacticList[Tactic::Pass]                = new TPass(state, botID);
		tacticList[Tactic::Kickoff]         = new TKickoff(state, botID);
		tacticList[Tactic::Position]            = new TPosition(state, botID);
		tacticList[Tactic::Defend]              = new TDefend(state,botID);
		tacticList[Tactic::Attack]              = new TAttack(state,botID);
		tacticList[Tactic::SupportAttack2015]	= new TSupportAttack2015(state,botID);
		tacticList[Tactic::Stop]                = new TStop(state, botID);
		tacticList[Tactic::Velocity]            = new TVelocity(state, botID);
		tacticList[Tactic::Backup]              = new TBackup(state, botID);
		tacticList[Tactic::ReceiveBall]         = new TReceiveBall(state, botID);
		tacticList[Tactic::ReceiveSpline]         = new TReceiveSpline(state, botID);
		tacticList[Tactic::TestgotoPoint] 			= new Testgotopoint(state,botID);
		tacticList[Tactic::TestbotRace] 				= new TestbotRace(state,botID);
		tacticList[Tactic::GoalKick]						= new TGoalKick(state,botID);
		tacticList[Tactic::Shoot]                  = new TShoot(state,botID);
		tacticList[Tactic::Defender2]		   = new TDefender2(state, botID);
		tacticList[Tactic::Goalie2]		   = new TGoalie2(state, botID);	
		tacticList[Tactic::DWDefender2015]		   = new TDWDefender2015(state, botID);
		tacticList[Tactic::CoverGoal2015]           = new TCoverGoal2015(state,botID);
		tacticList[Tactic::CoverGoalPairLeft]           = new TCoverGoalPairLeft(state,botID);
		tacticList[Tactic::CoverGoalPairRight]           = new TCoverGoalPairRight(state,botID);
		tacticList[Tactic::ReceiveBall2015]         = new TReceiveBall2015(state, botID);
		tacticList[Tactic::AttackSpline]			   = new TAttackSpline(state, botID);
		tacticList[Tactic::Attack2015]			   = new TAttack2015(state, botID);
    // Initialization check
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
      assert(tacticList[tID] != NULL); // Tactic enum not mapped to the corresponding Tactic object
    }
    this->start();
  } // Robot

  Robot::~Robot()
  {
    delete state;
    
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
      if (tacticList[tID] != NULL)
      {
        delete tacticList[tID];
      }
    }
  } // ~Robot

  void Robot::run()
  {
    Tactic::ID    tID = Tactic::Stop;
    Tactic::Param tParam;
    bool          updated;

    while (true)
    {
      updated = false;
      // Updating the belief state
      // Reader with Writer preference code (Entry)
      writer_preference->enter();
      writer_preference->leave();
      reader_mutex->enter();
      if(reader_count == 0)
        writer_mutex->enter(); 
      reader_count++;
      reader_mutex->leave();
      // Reader (Entry ends)
      if (state->currFrameNum != stateSh.currFrameNum)
      {
        updated = true;
        memcpy(state, &stateSh, sizeof(BeliefState));
      }
      // Reader (Exit)
      reader_mutex->enter();
      if(reader_count == 1)
        writer_mutex->leave();
      reader_count--;
      reader_mutex->leave();
      // Reader (Exit ends)

      // Updating the tactic and its parameters to be executed next
      tacticCS->enter();      
      if (tStateSh == Tactic::INIT)
      {
				tID    = tIDSh;
				tParam = tParamSh;
        tStateSh = Tactic::RUNNING;
        tacticList[tID]->initialize();
      }
      tacticCS->leave();

      if (updated)
      {
        tacticList[tID]->execute(tParam);
      }
      sleep(16);  // Adding sleep to this thread of execution to prevent CPU hogging
    }
  } // run
} // namespace Strategy
