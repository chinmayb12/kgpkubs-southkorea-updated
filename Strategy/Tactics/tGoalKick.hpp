#ifndef TGOALKICK_HPP
#define TGOALKICK_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include <sys/time.h>
#include <unistd.h>
#include <vector>
using namespace std;
namespace Strategy
{
  class TGoalKick: public Tactic
  {
  public:

    TGoalKick(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {

    } // TPosition

    ~TGoalKick()
    { } // ~TPosition
    inline bool isActiveTactic(void) const
    {
      return false;
    }
    //CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
    //Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
     int minv = *(freeBots.begin());
      int mindis = 1000000000;
      Point2D<int> goalPos(ForwardX(-(HALF_FIELD_MAXX)), 0);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
                const int factor = 0.2;
        float perpend_dist = ForwardX(state->homePos[*it].x - ForwardX(-HALF_FIELD_MAXX));
                Vector2D<int> goal;
                goal.x = OUR_GOAL_X;
                goal.y = 0;
                float dist_from_goal = Vector2D<int>::dist(state->homePos[*it], goal);
                
                if(dist_from_goal + factor * perpend_dist < mindis)
        {
          mindis = dist_from_goal + factor * perpend_dist;
          minv = *it;
        }
      }
	  	
      return minv;
    } // chooseBestBot
	int goalieId(){
		int id=0;
		float dist1, dist2;
		Point2D<int> ourGoalMidPoint(ForwardX(-HALF_FIELD_MAXX), 0);
		dist1=Point2D<int>::dist(state->homePos[id], ourGoalMidPoint);
		for (int i=1;i<5;i++){
			dist2=Point2D<int>::dist(state->homePos[i], ourGoalMidPoint);
			if(dist2 <dist1){
				id=i;
				dist1=dist2;
			}
		}
		return id;
  }
    void execute(const Param& tParam)
    {
			static bool flag = false;
			static int ID=goalieId();
			float theta = normalizeAngle(Vector2D<int>::angle(state->ballPos,state->homePos[ID]));
			float turnAngleLeft = normalizeAngle(theta - state->homeAngle[ID]); // Angle left to turn
			if(turnAngleLeft>PI/2||turnAngleLeft<-PI/2)
			{
             if(turnAngleLeft>PI/2)
              turnAngleLeft=turnAngleLeft-PI;
             else
               turnAngleLeft=turnAngleLeft+PI;
			}
			
			printf("\n Bot Goalie : %d",botID);
			if(fabs(turnAngleLeft)<0.7*DRIBBLER_BALL_ANGLE_RANGE)
				flag =true;
				
			if(flag==false)
			{
				printf("\n turning %f \n",turnAngleLeft);
				sID = SkillSet::TurnToAngle;
				sParam.TurnToAngleP.finalslope = Vector2D<int>::angle(state->ballPos,state->homePos[ID]);
			}
			
			else
			{
				printf("\n going to ball\n");
				sID = SkillSet::GoToPoint;
				sParam.GoToPointP.x = state->ballPos.x;
				sParam.GoToPointP.y = state->ballPos.y;
				sParam.GoToPointP.increaseSpeed = true;
			}
			skillSet->executeSkill(sID, sParam);
		}
		

  }; // class TPosition
} // namespace Strategy

#endif // TPOSITION_HPP