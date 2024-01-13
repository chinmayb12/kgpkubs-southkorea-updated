#ifndef TPOSITION_HPP
#define TPOSITION_HPP

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
  class TPosition: public Tactic
  {
  public:

    TPosition(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {

    } // TPosition

    ~TPosition()
    { } // ~TPosition
    inline bool isActiveTactic(void) const
    {
      return false;
    }
    //CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
    //Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      assert(tParam != 0);
      int minv   = *(freeBots.begin());
      int mindis = 1000000000;
      Vector2D<int> tGoToPoint(tParam->PositionP.x, tParam->PositionP.y);
      
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_point = (state->homePos[*it] - tGoToPoint).absSq();
        if(dis_from_point < mindis)
        {
          mindis = dis_from_point;
          minv = *it;
        }
      }
      printf("%d assigned Position\n", minv);
      return minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      printf("Position BotID: %d\n",botID);
      vector<pair<float, float> > obstacles;
      Vector2D<int> oppGoalCenter(ForwardX(HALF_FIELD_MAXX), 0);
      getFullAngularClearance(obstacles, Vector2D<int>::dist(state->homePos[botID], oppGoalCenter)-1, state->homePos[botID]);
      //cout<<obstacles.size()<<endl;
      /*for(vector<pair<float, float> >::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
        SkillSet::comm->addLine(state->homePos[botID].x, state->homePos[botID].y, 
                    state->homePos[botID].x + 10000*cos(it->first), state->homePos[botID].y + 10000*sin(it->first), 0xFF0000);
        SkillSet::comm->addLine(state->homePos[botID].x, state->homePos[botID].y, 
                    state->homePos[botID].x + 10000*cos(it->second), state->homePos[botID].y + 10000*sin(it->second), 0x00FF00);
      }*/
      pair<float, float> res;
      getOppGoalClearance(obstacles, state->homePos[botID], res);
      SkillSet::comm->addLine(state->homePos[botID].x, state->homePos[botID].y, 
                  state->homePos[botID].x + 10000*cos((res.first+res.second)/2), state->homePos[botID].y + 10000*sin((res.first+res.second)/2), 0x00FFF0);

      // Select the skill to the executed next
      gotoPointExact(tParam.PositionP.x, tParam.PositionP.y, tParam.PositionP.align, tParam.PositionP.finalSlope, tParam.PositionP.finalVelocity);
    }
  }; // class TPosition
} // namespace Strategy

#endif // TPOSITION_HPP
