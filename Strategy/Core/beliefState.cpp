#include <ctime>
#include "beliefState.h"
#include "visionThread.h"
#include "config.h"
#include "thread.h"
#include "rbCommDef.h"
#include "cs.hpp"
#include "config.h"
#include "logger.h"
#include "skillSet.h"

using namespace std;
using namespace Util;
using namespace HAL;
using namespace Simulator;

namespace Strategy
{
  BeliefState::BeliefState(const BeliefState &s) {
    *this = s;
  }
  BeliefState::BeliefState() :
    prevFrameNum(-1),
    currFrameNum(-1)
  {
    for(int botID = 0; botID < HomeTeam::SIZE; botID++)
    {
      homeAngle[botID] = 0;
    }

    for(int botID = 0; botID < AwayTeam::SIZE; botID++)
    {
      awayAngle[botID] = 0;
    }
    ourGoalCount    = 0;
    oppGoalCount    = 0;
    timeRemaining   = 0;
    // If Refree Box is not going to run do not wait for signal from refree box to start the game
#ifdef RUN_REFBOX
    pr_gameRunning     = false;
#else
    pr_gameRunning     = true;
#endif
    pr_gameHalted      = false;
    
    //Refree Box
    pr_ourGoalScored = false;
    pr_oppGoalScored = false;
    
    pr_ourFreeKick     = false;
    pr_oppFreeKick     = false;
		pr_ourFreeBall     = false;
		pr_oppFreeBall     = false;
    pr_ourGoalKick     = false;
    pr_oppGoalKick     = false;
    pr_ourPenaltyKick = false;
    pr_oppPenaltyKick = false;
    pr_ourKickOff = false;
    pr_oppKickOff = false;
    pr_ourKickOffStart = false;
		pr_ourPenaltyKickStart = false;
		
    //Predicates
    pr_ball_in_our_dbox = false;
    pr_ball_in_opp_dbox = false;
    pr_offense = false;
    pr_defense = false;
    pr_oppBall = false;
    pr_ourBall = false;
    pr_looseBall = false;
    pr_ballOppSide = false;
    pr_ballOurSide = false;
    pr_ballMidField = false;
    pr_balInOurCorner = false;
    pr_ballInOppCorner = false;
    pr_ballInSideStrip = false;
    pr_ballInFrontStrip = false;
	pr_ballHasCollided = false;
  }

  BeliefState::~BeliefState()
  {
    //printf("<><><><>destroy!!!\n");
  }

  bool BeliefState::update()
  {
    pr_ourKickOffStart = false;
		pr_ourPenaltyKickStart = false;
    computeBallLocation();
    computeBallInDBox();
    computeBallOurSide();
    computeBallOppSide();
    computeBallInStrips();
    computeBallMidfield();
	  ballHasCollided();
    return true;
  } // update
  /* Function definitions for all the predicates...
     * */

  void BeliefState::computeBallLocation()
  {
    int homeNearbotID = -1, awayNearbotID = -1;
    float minDist, distSq, closestBot ;
    minDist = 1e20;
    for(int i = 0; i < HomeTeam::SIZE ; i++)
    {
      float th = Vector2D<int>::angle(ballPos, homePos[i]);
      float theta = firaNormalizeAngle(th - homeAngle[i]);
      distSq =  Vector2D<int>::distSq(ballPos , homePos[i]);

      if(distSq < minDist)
      {
        minDist = distSq;
        closestBot = i;
      }
      this->ourBotNearestToBall = closestBot;
      //printf("Vals b %f \t %f \t %f\n",distSq,theta,2*DRIBBLER_BALL_ANGLE_RANGE);
      
      if(distSq < (BOT_BALL_THRESH * BOT_BALL_THRESH) && fabs(theta) < 2*DRIBBLER_BALL_ANGLE_RANGE)
      {
        homeNearbotID = i;
        break ;
      }
    }
    minDist = 1e20;
    for(int i = 0; i < AwayTeam::SIZE ; i++)
    {
      float theta = firaNormalizeAngle(Vector2D<int>::angle(ballPos, homePos[i]) - awayAngle[i]);
      distSq = Vector2D<int>::distSq(ballPos , awayPos[i]);
      if(distSq < minDist)
      {
        minDist = distSq;
        closestBot = i;
      }
      this->oppBotNearestToBall = closestBot;
      if(distSq < (BOT_BALL_THRESH * BOT_BALL_THRESH) && fabs(theta) < 2*DRIBBLER_BALL_ANGLE_RANGE)
      {
        awayNearbotID = i;
        break ;
      }
    }
    //printf("OurBotnearest, OppBotNearest : %d %d\n",ourBotNearestToBall,oppBotNearestToBall);
    if(awayNearbotID == -1 && homeNearbotID == -1)
    {
      pr_looseBall = true;
      pr_oppBall = false;
      pr_ourBall = false;

    }
    else if(awayNearbotID == -1 && homeNearbotID != -1)
    {
      pr_looseBall  =  false;
      pr_oppBall    =  false;
      pr_ourBall    =  true;
    }
    else if(awayNearbotID != -1 && homeNearbotID == -1)
    {
      pr_looseBall  =  false;
      pr_oppBall    =  true;
      pr_ourBall    =  false;
    }
    else if(awayNearbotID != -1 && homeNearbotID != -1)
    {
      pr_looseBall  =  false;
      pr_oppBall    =  true;
      pr_ourBall    =  false;

      //TODO: Incorporate bot orientation to decide if homeBot is pointing in the right direction
    }

  }

  void BeliefState::computeBallInDBox()
  {
    pr_ball_in_opp_dbox = false;
    pr_ball_in_our_dbox = false;

    if(ForwardX((int)ballPos.x) > (HALF_FIELD_MAXX - DBOX_WIDTH) &&
        ballPos.y < DBOX_HEIGHT &&
        ballPos.y > -DBOX_HEIGHT)
      pr_ball_in_opp_dbox = true;
    else if(ForwardX((int)ballPos.x) < (-HALF_FIELD_MAXX + DBOX_WIDTH) &&
            ballPos.y < DBOX_HEIGHT &&
            ballPos.y > -DBOX_HEIGHT)
      pr_ball_in_our_dbox = true;
  }

  //The following three assume that 0,0 is at center of frame (for checking ball parameters)

  void BeliefState::computeBallOppSide()
  {
    if(ForwardX(ballPos.x) > MID_FIELD_THRESH)
    {
      pr_ballOurSide = false;
      pr_ballOppSide = true;
    }
  }

/**
 * @brief 
 */
  void BeliefState::computeBallOurSide()
  {
    if(ForwardX(ballPos.x) < -MID_FIELD_THRESH)
    {
      pr_ballOurSide = true;
      pr_ballOppSide = false;
    }
  }

  void BeliefState::computeBallMidfield()
  {

    if(ForwardX(ballPos.x) > -MID_FIELD_THRESH && ForwardX(ballPos.x) < MID_FIELD_THRESH)
    {
      pr_ballMidField = true;
    }
  }

  bool BeliefState::pr_nOpponentsOurSide(int n) const
  {
    int ctr = 0;
    for(int i = 0; i < AwayTeam::SIZE ; i++)
    {
      if(ForwardX(awayPos[i].x) < 0)
        ctr ++ ;
    }
    if(ctr >= n)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballXGt(int x)
  {
    if(ballPos.x > x)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballXLt(int x)
  {
    if(ballPos.x  < x)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballAbsYGt(int y)
  {
    int absy = ballPos.y > 0 ? ballPos.y : (-(ballPos.y)) ;
    if(absy > y)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballAbsYLt(int y)
  {
    int absy = ballPos.y > 0 ? ballPos.y : (-(ballPos.y)) ;
    if(absy < y)
      return true ;
    else
      return false ;
  }

  void BeliefState::computeBallInStrips()
  {
    pr_ballInFrontStrip = (ballPos.x > HALF_FIELD_MAXX - STRIP_WIDTH_X || ballPos.x < -HALF_FIELD_MAXX + STRIP_WIDTH_X);
    pr_ballInSideStrip  = (ballPos.y > HALF_FIELD_MAXY - STRIP_WIDTH_Y || ballPos.y < -HALF_FIELD_MAXY + STRIP_WIDTH_Y);
  }

  void BeliefState::updateStateFromRefree(HAL::RefBoxCmd &refBoxCmdSh)
  {
		bool pr_ourKickOff_prev = pr_ourKickOff;
		bool pr_ourPenaltyKick_prev = pr_ourPenaltyKick;
		bool pr_ourGoalKick_prev = pr_ourGoalKick;
    resetPr();

    if(HomeTeam::COLOR == Simulator::BLUE_TEAM) {
      if(ourGoalCount != refBoxCmdSh.goalsBlue)
        pr_ourGoalScored = true;
      if(oppGoalCount != refBoxCmdSh.goalsYellow)
        pr_oppGoalScored = true;
      ourGoalCount   = refBoxCmdSh.goalsBlue;
      oppGoalCount   = refBoxCmdSh.goalsYellow;
    } else {
      if(ourGoalCount != refBoxCmdSh.goalsYellow)
        pr_ourGoalScored = true;
      if(oppGoalCount != refBoxCmdSh.goalsBlue)
        pr_oppGoalScored = true;
      ourGoalCount   = refBoxCmdSh.goalsYellow;
      oppGoalCount   = refBoxCmdSh.goalsBlue;
    }
    timeRemaining  = refBoxCmdSh.timeRemaining;
    // update state based on the value of the cmd. Need to define the conventions.
    //oppGoalCount   = refBoxCmdSh.goalsYellow;
    //ourGoalCount   = refBoxCmdSh.goalsBlue;
    timeRemaining  = refBoxCmdSh.timeRemaining;
    Util::Logger::toStdOut("Updating state from REFEREE BOX");
    switch(refBoxCmdSh.cmd)
    {
      case 'S':
        pr_gameRunning = false;
        break;
        
      case 'H':
        pr_gameRunning = false;
        pr_gameHalted = true;
        break;
        
      case 's':
        pr_gameRunning = true;
        pr_ourGoalScored = false;
        pr_oppGoalScored = false;
				if(pr_ourKickOff_prev) {
					pr_ourKickOffStart = true;
				} else if(pr_ourPenaltyKick_prev) {
					pr_ourPenaltyKickStart = true;
				} else if(pr_ourGoalKick_prev) {
					
				}
        break;
        
      case 'K':
        pr_ourKickOff = true;
        break;
				
			case 'Y':
				pr_ourFreeBall = true;
				break;
				
			case 'y':
				pr_oppFreeBall = true;
				break;
				
      case 'k':
        pr_oppKickOff = true;
        break;
        
      case 'p':
        pr_oppPenaltyKick = true;
        break;
        
      case 'f':
        pr_oppFreeKick = true;
        break;
        
      case 'i':
        pr_oppGoalKick = true;
        break;
        
      case 'g':
        pr_oppGoalScored = true;
        break;
      
      case 'G':
        pr_ourGoalScored = true;
        break;
        
      case 'P':
        pr_ourPenaltyKick = true;
        break;
        
      case 'F':
        pr_ourFreeKick = true;
        break;
        
      case 'I':
        pr_ourGoalKick = true;
        break;
        
      case 'n':
        pr_ourFreeKick = false;
        pr_oppFreeKick = false;
        pr_ourGoalKick = false;
        pr_oppGoalKick = false;
        pr_gameRunning = true;
        break;
    }
    refreeUpdated = true;
  }
  
  int BeliefState::ourBotNearestToPoint(const int x, const int y) {
    float minDis  = 1e20;
    int   botId   = -1;
    for(int i = 0; i < HomeTeam::SIZE; i++) {
      float dnew = Vector2D<int>::dist(homePos[i], x, y);
      if(dnew < minDis) {
        minDis = dnew;
        botId  = i;
      }
    }
    return botId;
  }
  
  int BeliefState::oppBotNearestToPoint(const int x, const int y) {
    float minDis  = 1e20;
    int   botId   = -1;
    for(int i = 0; i < AwayTeam::SIZE; i++) {
      float dnew = Vector2D<int>::dist(awayPos[i], x, y);
      if(dnew < minDis) {
        minDis = dnew;
        botId  = i;
      }
    }
    return botId;
  }
  
  void BeliefState::ballHasCollided(){
	 if(abs(ballVel.y - prevBallVel.y) > 100){
			if(SGN(ballVel.y)*SGN(prevBallVel.y) == -1)
				pr_ballHasCollided = true;
	}	 
    if(abs(ballVel.x - prevBallVel.x) > 100){
			if(SGN(ballVel.x)*SGN(prevBallVel.x) == -1){
				pr_ballHasCollided = true;
			}
	}	
  }
  
  void BeliefState::resetPr()
  {
    //R_Box
    pr_ourGoalScored = false;
    pr_oppGoalScored = false;
    
    pr_ourFreeKick     = false;
    pr_oppFreeKick     = false;
		pr_ourFreeBall     = false;
		pr_oppFreeBall     = false;
    pr_ourGoalKick     = false;
    pr_oppGoalKick     = false;
    pr_ourPenaltyKick = false;
    pr_oppPenaltyKick = false;
    pr_ourKickOff = false;
    pr_oppKickOff = false;
    pr_ourKickOffStart = false;
		pr_ourPenaltyKickStart = false;
    
    //Predicates
    pr_ball_in_our_dbox = false;
    pr_ball_in_opp_dbox = false;
    pr_offense = false;
    pr_defense = false;
    pr_oppBall = false;
    pr_ourBall = false;
    pr_looseBall = false;
    pr_ballMidField = false;
    pr_balInOurCorner = false;
    pr_ballInOppCorner = false;
    pr_ballInSideStrip = false;
    pr_ballInFrontStrip = false;
	pr_ballHasCollided = false;
  }
  
} // namespace Strategy
