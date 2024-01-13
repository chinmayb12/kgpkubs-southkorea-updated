#ifndef PLAY_HPP
#define PLAY_HPP

#include <utility>
#include <vector>
#include <string>
#include "beliefState.h"
#include "robot.h"
#include "timer.h"
#include "logger.h"

namespace Strategy
{
  class Play
  {
    friend class PS;

  public:
    enum Result
    {
      SUCCESS_LOW,//0
      SUCCESS_MED,//1
      SUCCESS_HIGH,//2
      FAILURE_LOW,//3
      FAILURE_MED,//4
      FAILURE_HIGH,//5
      COMPLETED,//6
      TIMED_OUT,//7
      NOT_TERMINATED//8
    }; // enum Result
    inline bool timedOut()
    {
      //printf("Time left: %d\n", timer.split() - timeoutPeriod);
      int tm = timer.split();
	 //printf(" tm :: %d :::: tm - las :: %d ::::: role %d \n",tm,tm - lastRoleReassignTime,roleReassignPeriod);
     //sleep(5);
	 if((tm - lastRoleReassignTime) > roleReassignPeriod) {
        lastRoleReassignTime = tm;
        shouldRolesReassign  = true;
      }
	  cout << "Return: " << (tm>=timeoutPeriod) << " Value of timeoutPeriod " << timeoutPeriod ;
      return (tm >= timeoutPeriod);
    }
    enum PlayType
    {
      PLAYTYPE_YES,
      PLAYTYPE_NO,
      PLAYTYPE_DONTCARE
    };
  private:
    // Default timeout period of any play
    static const int DEFAULT_TIMEOUT_PERIOD = 60;

    Util::Timer timer;
    int         timeoutPeriod; // in milliseconds
    int         roleReassignPeriod;
    int         lastRoleReassignTime;

    // Sets the timeout period for the play in seconds
    inline void startTimer(void)
    {
      timer.start();
			int tm = timer.split();
      lastRoleReassignTime = tm - 2*roleReassignPeriod; //so that 1st call to timedOut gives shouldRolesReassign = true. 2* just to be safe
    }

  protected:
    Result             result;  // Assigned when the play terminates
    const BeliefState& state;

    // Sets the timeout period of the play in seconds
    void setTimeout(double timeoutPeriod, int roleReassignPeriod = 1)
    {
      this->timeoutPeriod = (timeoutPeriod * 1000);
      this->roleReassignPeriod = roleReassignPeriod * 1000;
    }

    inline void computeMaxTacticTransits()
    {
      maxTacticsPerRole = roleList[0].size();
      for (int roleIdx = 1; roleIdx < HomeTeam::SIZE; ++roleIdx)
      {
        int sz = roleList[roleIdx].size();
        if (roleList[roleIdx].size() > maxTacticsPerRole)
        {
          maxTacticsPerRole = sz;
        }
      }
    }

	public:
   
   //search for opponent bot if its at desireed postion #positional plays
 
  /*
  queue < Vector2D<int> > vpoint;
  vpoint.push(dpoint);
  Vector2D<int> point = dpoint;
  while(vpoint.size()!=0)
  {	
   if(search_opp_bot(state,vpoint.front()))
    {
	  dpoint = vpoint.front();
	  point.x = dpoint.x + 2*BOT_RADIUS ; point.y =  dpoint.y ;
	  if((abs(dpoint.x)<HALF_FIELD_MAXX)&&(abs(dpoint.x - state->ballPos.x)>KICK_THRESH))
		vpoint.push(point);
	  point.x = dpoint.x - 2*BOT_RADIUS ; point.y =  dpoint.y;
	  if((abs(point.x)<HALF_FIELD_MAXX)&&(abs(point.x - state->ballPos.x)>KICK_THRESH))
		vpoint.push(point); 
      point.x = dpoint.x ; point.y =  dpoint.y + 2*BOT_RADIUS ;
      if((abs(point.y)<HALF_FIELD_MAXY)&&(abs(point.y - state->ballPos.y)>KICK_THRESH))
		vpoint.push(point);    
	  point.x = dpoint.x ; point.y =  dpoint.y - 2*BOT_RADIUS;
      if((abs(point.y)<HALF_FIELD_MAXY)&&(abs(point.y - state->ballPos.y)>KICK_THRESH))
		vpoint.push(point);    	
		
		vpoint.pop();
     }
    else
	{ 
	  return vpoint.front();
	  break;
	}
  }
}	
   */
bool position_in_dbox(const BeliefState* state,Vector2D<int> dpoint)
{
if(((abs(dpoint.x)<HALF_FIELD_MAXX)&&(abs(dpoint.x)>(HALF_FIELD_MAXX - DBOX_WIDTH)))&&(abs(dpoint.y)<DBOX_HEIGHT))	
  return true ;
else 
  return false ;  
}

 bool search_opp_bot(const BeliefState* state,Vector2D<int> dpoint) 
 {  
	 //returns true if the point has an already occupied opp bot (invalid point)
	  int i = 0 ;
	  float dist1 ;
  if((abs(dpoint.x)>HALF_FIELD_MAXX)||(abs(dpoint.y)>HALF_FIELD_MAXY))
	  return true ;
 
 while(i<5)
   {
	 dist1 = Vector2D<int>::dist(dpoint,state->awayPos[i]);
	 if(dist1< 2*BOT_BALL_THRESH)
		 return true;
     i++;   
   }
  return false;	
	  
  }
  
Vector2D<int> position_correcter(const BeliefState* state,Vector2D<int> dpoint)
{
float dist ;
 if((abs(dpoint.x)>HALF_FIELD_MAXX)&&(abs(dpoint.y)>HALF_FIELD_MAXY))	
  {
	if(abs(dpoint.x)>HALF_FIELD_MAXX)
	  dpoint.x = SGN(dpoint.x)*HALF_FIELD_MAXX ;
	
	if(abs(dpoint.y)>HALF_FIELD_MAXY)
		dpoint.y = SGN(dpoint.y)*HALF_FIELD_MAXY ;
	
  }
  
    if(position_in_dbox(state,dpoint))
	{
	   dpoint.x = SGN(dpoint.x)*(HALF_FIELD_MAXX- DBOX_WIDTH) ;
	}
    
	return dpoint;
  
}

Vector2D<int> point_generate(const BeliefState* state,Vector2D<int> point , int i )
{   //generates the points in circular manner 
int d = (i/4 + 1)*3*BOT_RADIUS;	
  switch(i%4)
  {
	case 0 :
	 point.x += d;
	 break;
	case 1 :
	 point.y += d;
	 break;
	case 2 :
	 point.x += -d;
	break;
	case 3 :
	 point.y += -d;
     break;
  }
  return point;
}

bool valid_point(const BeliefState* state,Vector2D<int> point,float kick_thresh)  
{ 
	//return true if the the point is in the field and with kick thresh
	if((fabs(point.x)<HALF_FIELD_MAXX)&&(fabs(point.y)<HALF_FIELD_MAXY)&&((Vector2D<int>::dist(point,state->ballPos))>kick_thresh))
	   return true;
	else 
	  return false;
}

//called in the positional plays
Vector2D<int> position_our_bot(const BeliefState* state,Vector2D<int> dpoint, float kick_thresh)
{
int i = 0 ;
Vector2D<int> vpoint = dpoint ;
 while((search_opp_bot(state,vpoint))&&(!valid_point(state,vpoint,kick_thresh)))
 {
  vpoint = point_generate(state,dpoint,i);
  i++;
  if(i>8)break;
 }
 return vpoint ;
}


    bool        shouldRolesReassign;
	virtual void reevaluateRoleParams() {} //Function to reevaluate the params for each role in every call of executePlay() in pExec
    inline Play(const BeliefState& state) :
      timeoutPeriod(Play::DEFAULT_TIMEOUT_PERIOD),
      result(NOT_TERMINATED),
      state(state),
      weight(1.0f),
      maxTacticsPerRole(0)
    {
      name = "";
      setTimeout(60);
    }

    // Name of the play
    std::string name;
    PlayType PositionPlay;
    PlayType AttackPlay;
    
    // Weight of the play - Measure of its applicability in the current scenario
    float weight;

    // roleList is a vector of pairs of Tactic ID and Tactic Parameter
    std::vector<std::pair<Tactic::ID, Tactic::Param> > roleList[HomeTeam::SIZE];
    int assignedBot[HomeTeam::SIZE];
    unsigned int maxTacticsPerRole;

    // Returns true if the play is applicable otherwise false
    virtual bool applicable(void) const {
      if((state.pr_gameRunning && this->PositionPlay == PLAYTYPE_YES)|| (!state.pr_gameRunning && this->PositionPlay == PLAYTYPE_NO)) {
        //printf("%d %d", state.gameRunning, this->PositionPlay);
        return false;
      }
      return true;
    }

    /* Returns one of SUCCESS/FAILURE/ABORTED/COMPLETED as applicable if the play terminates
     * otherwise NO_TERMINATION
     */
    virtual Result done(void) const = 0;
  }; // class Play
} // namespace Strategy

#endif // PLAY_HPP