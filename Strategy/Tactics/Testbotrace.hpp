// Change the FOLDER DIR. as per ur computer

#ifndef TESTBOTRACE_HPP
#define TESTBOTRACE_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

namespace Strategy
{
  class TestbotRace : public Tactic
  {
  public:
    TestbotRace(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { 
    } 

    ~TestbotRace()
    { } 
    inline bool isActiveTactic(void) const
    {
      return true;
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
      
		static int count =1;
		static bool reset = false;

		freopen ("/home/robo/FIRA_BOT_TEST/Test_botrace.txt","a+",stdout);		// Change the FOLDER DIR. as per ur computer
		Vector2D<int> spoint,dpoint,ref;
		float dist;
		spoint.x = ForwardX(-(HALF_FIELD_MAXX-DBOX_WIDTH));
		spoint.y=0;
		ref.x =1;ref.y=0;
		dpoint.x = dpoint.y=0;
		
		if(reset==false)
		gotoPoint(spoint.x, spoint.y, true, Vector2D<int>::angle(dpoint, state->homePos[botID]), 0);
		
		if( ( (Vector2D<int>::dist(spoint, state->homePos[botID]) < BOT_BALL_THRESH) && abs( state->homeVel[botID].x)<5 && abs(state->homeVel[botID].y)<5 &&  Vector2D<int>::angle(dpoint, state->homePos[botID])<0.5 ) || reset)
			 {
				 reset = true;
			//	printf("\n asdasda \n");.
				static clock_t tim=clock();
				static int countit =1;
				static bool reach = false;
				
			    // Select the skill to the executed next
				sID = SkillSet::Velocity;
				sParam.VelocityP.vl = tParam.TestbotRaceP.vl;
				sParam.VelocityP.vr = tParam.TestbotRaceP.vr;      
				// Execute the selected skill
				skillSet->executeSkill(sID, sParam);
				
				dist = Vector2D<int>::dist(dpoint, state->homePos[botID]);  
				if(state->homePos[botID].x>=dpoint.x || reach==true )
				{
					reach = true;
					//printf("im here\n\n");
					  stopBot();
				}
				
				if(fabs(state->homeVel[botID].x)<10 && fabs(state->homeVel[botID].y)<10 && reach==true)
				{
				//	printf("\ndone\n %d",countit);
					if(count==1)
					printf("\n No.       ERROR         ERROR%      			   TIME \n");
					//dpoint.y = state->homePos[botID].y;
					dist = Vector2D<int>::dist(dpoint, state->homePos[botID]);  
					clock_t sec = clock()-tim;
					//printf("\n done");
					printf("\n %d    :    %f      %f   	%f",count,dist,(dist/HALF_FIELD_MAXX)*100,((float)sec)/CLOCKS_PER_SEC );
					tim = clock();
					count++;
					reset = false;
					reach = false;
				} 
		   
			

				}
		if (count==20)
      {
        tState = COMPLETED;
      }
    }
  };
} // namespace Strategy

#endif 
