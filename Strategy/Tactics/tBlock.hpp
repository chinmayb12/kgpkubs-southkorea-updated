#ifndef TTBLOCK_HPP
#define TTBLOCK_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"

#define TOTAL_BOTS 4 // No. of opponent Bots
#define RADIUS_REBOUND tParam.BlockP.dist    // The distance at which bot should wait for rebound

namespace Strategy
{
  class TBlock : public Tactic
  {
  public:
    TBlock(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TBlock

    ~TBlock()
    { } // ~TBlock

    inline bool isActiveTactic(void) const
    {
      return false;
    }
	
	//ChooseBestBot
	/*			Commented. Test remaining
	    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
		bool choose_state = 1;
      int minv =  (freeBots.begin());
	  int minv_t = (freeBots.begin());
      int mindis = 10000000;
	  int mindis_t = 10000000;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        if(ForwardX(state->homePos[*it].x) < ForwardX(state->ballPos.x) && Vector2D<int>::dist(state->homePos[*it], state->ballPos) < mindis)
        {
          mindis = Vector2D<int>::dist(state->homePos[*it], state->ballPos);;
          minv = *it;
		  choose_state = 0;
        }
		else if( choose_state && Vector2D<int>::dist(state->homePos[*it], state->ballPos) < mindis_t)
		{
			mindis_t = Vector2D<int>::dist(state->homePos[*it], state->ballPos);
          minv_t = *it;
		}
      }
	  if( choose_state)
		  return minv_t;
      return minv;
    } // chooseBestBot
	*/
	
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10000000;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        if(ForwardX(state->homePos[*it].x) < ForwardX(state->ballPos.x) && Vector2D<int>::dist(state->homePos[*it], state->ballPos) < mindis)
        {
          mindis = Vector2D<int>::dist(state->homePos[*it], state->ballPos);;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot
    void execute(const Param& tParam)
    {
      printf("Block BotID: %d\n",botID);
        
        /*
         * This tactic tries to position itself in the path of the ball until 
         * the ball is at a particular distance from the goal,then it position 
         * itself to receive the possible rebound
         */
      //printf("Bot Pos : x -> %d  y -> %d\n",state->homePos[botID].x,state->homePos[botID].y);
     // if(state->ballVel.abs() < MIN_BOT_SPEED) return;    //Ball is almost at rest then peace
      SkillSet::SkillID sID;
      sID = SkillSet::GoToPoint;
      Strategy::SParam sParam;
      sParam.GoToPointP.align = false;
      //sParam.GoToPointP.finalslope = 0;
      float theta = Vector2D<int>::angle(state->homePos[botID],state->ballPos);
      float final_angle = state->homeAngle[botID] + theta;
      sParam.GoToPointP.finalslope = final_angle;
      //printf("Slope : %f\n",180*final_angle/PI);
      if(state->pr_ourBall){
        puts("OUR BALL");
          int enemyID = -1;
          float minDis = 100000000;
          for(int i=0;i < TOTAL_BOTS ; i++){
              float dis_from_ball = Vector2D<int>::dist(state->ballPos,state->awayPos[i]);
              if( dis_from_ball < minDis){
                  minDis = dis_from_ball;
                  enemyID = i;
              }  
          }
          if(enemyID == -1) return;   // No enemy :P
          
          float enemy_motion_angle = atan2(state->awayVel[enemyID].y,state->awayVel[enemyID].x);
          // This bot may come in our striker's way
          int dest_x = state->awayPos[enemyID].x + tParam.BlockP.dist * cos(enemy_motion_angle);
          int dest_y = state->awayPos[enemyID].y + tParam.BlockP.dist * sin(enemy_motion_angle);
          float theta = Vector2D<int>::angle(state->homePos[botID],state->ballPos);
          float final_angle = state->homeAngle[botID] + theta;
          sParam.GoToPointP.x = dest_x;
          sParam.GoToPointP.y = dest_y;
          sParam.GoToPointP.finalslope = 0 ;//final_angle;
					SkillSet::addCircle(sParam.GoToPointP.x,sParam.GoToPointP.y,50, 0x000000);
          skillSet->executeSkill(sID,sParam);
          return;
      }
      
      
      Vector2D<int> GoalPos((OUR_GOAL_X),-100);
     
      float ball_motion_angle = Vector2D<int>::angle(GoalPos,state->ballPos);               //atan2(state->ballVel.y, state->ballVel.x); // TODO: should I take the angle with goal instead?
      //ball_motion_angle += PI/2;
      int rCosTheta = (int)(tParam.BlockP.dist*cos(ball_motion_angle));
      int rSinTheta = (int)tParam.BlockP.dist*sin(ball_motion_angle);
    
      if( ForwardX(state->ballPos.x + rCosTheta) > (-HALF_FIELD_MAXX + 2*DBOX_WIDTH)){ //Check if it is correct else remove the abs and invert the inequality
            sParam.GoToPointP.x = (state->ballPos.x + rCosTheta);
            sParam.GoToPointP.y = state->ballPos.y + rSinTheta;
            if(sParam.GoToPointP.y > (HALF_FIELD_MAXY-500)){
              sParam.GoToPointP.y = HALF_FIELD_MAXY-500;
            }
            if(sParam.GoToPointP.y < (-HALF_FIELD_MAXY+500)){
              sParam.GoToPointP.y = -HALF_FIELD_MAXY+500;
            }
						SkillSet::addCircle(sParam.GoToPointP.x,sParam.GoToPointP.y,50, 0x000000);
            skillSet->executeSkill(sID,sParam);  //Taking position in the direction of the ball at some distance
      }
      else
      {
          int rebound_dist = 500;
          //Take position such that it may be receive the rebound
          float theta = (PI/2 - ball_motion_angle);    // Angle with the vertical
          int intersect_x = ForwardX(-HALF_FIELD_MAXX + 2 * DBOX_WIDTH);
          int intersect_y = (int)(state->ballPos.x - intersect_x) * tan(theta);
          int rCosTheta = (int)(rebound_dist*cos(-theta));
          int rSinTheta = (int)rebound_dist*sin(-theta);
          int dest_x = intersect_x + rCosTheta ;              //RADIUS_REBOUND * cos(-theta);
          int dest_y = intersect_y + rSinTheta;               //RADIUS_REBOUND * sin(-theta);
          sParam.GoToPointP.x = dest_x;
          sParam.GoToPointP.y = dest_y;
          //printf("Bot Pos : (%d,%d) Reflection : (%d,%d)\n",state->homePos[botID].x,state->homePos[botID].y,dest_x,dest_y);
          if(sParam.GoToPointP.y > (HALF_FIELD_MAXY-500))
          {
            sParam.GoToPointP.y = HALF_FIELD_MAXY-500;
          }
          if(sParam.GoToPointP.y < (-HALF_FIELD_MAXY+500))
            {
            sParam.GoToPointP.y = -HALF_FIELD_MAXY+500;
          }
          SkillSet::addCircle(sParam.GoToPointP.x,sParam.GoToPointP.y,50, 0x000000);
          skillSet->executeSkill(sID,sParam);
      }
      
      
//      else if(state->homePos[botID].x >/*Opp_D*/2300)  //end conditions
//      {
//        stopBot();
//        printf("Competed Tactic\n");
//        tState = COMPLETED;
//      }
    }
  };// class TBlock
} // namespace Strategy

#endif // TTBLOCK_HPP