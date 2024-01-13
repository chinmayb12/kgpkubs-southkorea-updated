#ifndef TTBACKUP_HPP
#define TTBACKUP_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "intersection.hpp"


//tBackup - by Sanyam
// place a bot in a vectorically opposite direction to the opponent bots
//the distance and direction is calculated as the harmonic sum(not sure if I just coined a word) of the opponent bots
//NOTE: Ball must be treated as origin

#define HP(B) (state->homePos[B])
#define AP(B) (state->awayPos[B])
#define BP    (state->ballPos) 
#define NUMBOTS (state->oppNum)
#define REDUCE_FIELD (.80)
#define ANGLE_TO_DIST 0

//harmonic sum calculator returns 1/sigma( 1/(oppbotbos[i]-ballPos) )
namespace Strategy
{
  class TBackup : public Tactic
  {
    public:
        TBackup(const BeliefState* state, int botID) :
      Tactic(Tactic::Backup, state, botID)
      {
        ;
      }
    #define tolerance (BOT_RADIUS/5)  // to prevent panic

        inline bool isActiveTactic(void) const
    {
      return true;
    }
     int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
        //returns the bot with minimum distance
        Vector2D<int> ball = BP;
        float mindist = 2*(HALF_FIELD_MAXX+HALF_FIELD_MAXY);
        int best_bot = 0;

      for(std::list<int>::iterator it=freeBots.begin(); it != freeBots.end();++it)
      {
        int radius;
        int i = *it;
        Vector2D<int> delta;
        delta.x =  HP(i).x - ball.x;
        delta.y =  HP(i).y - ball.y;
        radius = (int) sqrt( (delta.x)*(delta.x) + (delta.y)*(delta.y)  ); //euclidian distance THANKU :P mention not :P

        if( radius < mindist )
        {
          mindist = radius;
          best_bot = i;
        }

      }
                return best_bot;
        }


        float looking_angle( Vector2D<int> target )
        {
      int i = botID;
                Vector2D<int> ball = BP;
                Vector2D<int> delta;
                delta.x =  ball.x - HP(i).x;
                delta.y =  ball.y - HP(i).y;
                float angle = atan2(delta.y,delta.x);
                return angle;
        }
    
        int beyond_limit( Vector2D<int> target)
        {
    //NOTE: the .75 has been kept so that bot conforms to the current feild dimensions
                int px = ( target.x >= HALF_FIELD_MAXX*(REDUCE_FIELD)  );
                int nx = ( target.x <= -HALF_FIELD_MAXX*(REDUCE_FIELD)  );
                int py = ( target.y >= HALF_FIELD_MAXY*(REDUCE_FIELD)  );
                int ny = ( target.y <= -HALF_FIELD_MAXY*(REDUCE_FIELD)  );
                
                return (px||nx||py||ny);
        }
  
  Vector2D<int> harmonic()
  {
    float theta, radii =-1; float cur_radii,cur_theta;
                Vector2D<int> ball = BP;
    //printf("Numbots =  %d\n",NUMBOTS);
    if(NUMBOTS == 0)
    {
      Vector2D<int> ret(-HALF_FIELD_MAXX/2,0);
      return ret;
    }
	int bad_bots = 0;
	for(int i=0;i<NUMBOTS+bad_bots;i++)
	{
		/* new addition */
		Vector2D<int> opbot = AP(i);
		if( beyond_limit(opbot) )
		{
			bad_bots++;
			continue;
		}
			Vector2D<int> delta;
			delta.x =  AP(i).x - ball.x;
			delta.y =  AP(i).y - ball.y;
      
      //printf("opbot %d = (%d,%d)\n",i,AP(i).x,AP(i).y);
      //printf("ball (%d,%d)\n",ball.x,ball.y);
                        //theta[i] = atan2( delta.y, delta.x); //pricipal argument of line joining the oppbot and the ball
                        cur_radii = sqrt( (delta.x)*(delta.x) + (delta.y)*(delta.y)  ); //euclidian distance
      cur_theta = atan2( delta.y, ForwardX(delta.x) );
      if( (cur_radii< radii || radii < 0) && (fabs(cur_theta)<(PI/2) ) )
      {
        //update
        radii = cur_radii;
        theta = atan2( delta.y, delta.x);
      }

	}
    if(radii < 0)
    {
      Vector2D<int> ret(-HALF_FIELD_MAXX*(.25),0);
      return ret;
    }
    
    theta = normalizeAngle(PI+theta);
    //printf("radii = %f, theta = %f\n",radii,theta);
    Vector2D<int> retval; retval.x = BP.x + radii*cos(theta); retval.y = BP.y + radii*sin(theta);
	Vector2D<int> target = retval;
	int px = ( target.x >= HALF_FIELD_MAXX*(REDUCE_FIELD)  );
	int nx = ( target.x <= -HALF_FIELD_MAXX*(REDUCE_FIELD)  );
	int py = ( target.y >= HALF_FIELD_MAXY*(REDUCE_FIELD)  );
	int ny = ( target.y <= -HALF_FIELD_MAXY*(REDUCE_FIELD)  );

    if(px) target.x = HALF_FIELD_MAXX*(REDUCE_FIELD);
    else if(nx) target.x = -HALF_FIELD_MAXX*(REDUCE_FIELD);
    if(py) target.y = HALF_FIELD_MAXY*(REDUCE_FIELD);
    else if(ny) target.y = -HALF_FIELD_MAXY*(REDUCE_FIELD);
    return target;
  }

        /*Vector2D<int> harmonic()
        {
                float theta[NUMBOTS], radii[NUMBOTS];
                Vector2D<int> ball = BP;
    printf("Numbots =  %d\n",NUMBOTS);
                for(int i=0;i<NUMBOTS;i++)
                {
                        Vector2D<int> delta;
                        delta.x =  AP(i).x - ball.x;
                        delta.y =  AP(i).y - ball.y;
      printf("opbot %d = (%d,%d)\n",i,delta.x,delta.y);
                        theta[i] = atan2( delta.y, delta.x); //pricipal argument of line joining the oppbot and the ball
                        radii[i] = sqrt( (delta.x)*(delta.x) + (delta.y)*(delta.y)  ); //euclidian distance

                }

                Vector2D<float> invharmonic;
                invharmonic.x = 0;
                invharmonic.y = 0;

                for(int i = 0; i<NUMBOTS; i++)
                {
                        invharmonic.x += (1/(radii[i]+1))*cos(-theta[i]); // the +1 is to avoid division by zero
                        invharmonic.y += (1/(radii[i]+1))*sin(-theta[i]);
                }
    printf("invharmonic = (%f,%f)\n",invharmonic.x,invharmonic.y);
                float invradius = sqrt( (invharmonic.x)*(invharmonic.x) + (invharmonic.y)*(invharmonic.y) );
    printf("invradius  = %f\n",invradius);
                int radius = (int)(1/(invradius +.00001));
                float rtheta = atan2(invharmonic.y, invharmonic.x );

                Vector2D<int> retval;
    //float norm = (normalizeAngle(PI-rtheta));
    printf("Ball = (%d,%d), radius = %d\n",BP.x,BP.y,radius);
    
    retval.x = (int)( BP.x+radius*cos(normalizeAngle(PI-rtheta))  );
    retval.y = (int)( BP.y+radius*sin(normalizeAngle(PI-rtheta))  );
    
    printf("retval1 = %d, %d\n",retval.x,retval.y);
    
    while(beyond_limit(retval)&&radius>10)
    {
      radius *=.85;//each reduces radius by 15%
      retval.x = BP.x+radius*cos(normalizeAngle(PI-rtheta));
      retval.y = BP.y+radius*sin(normalizeAngle(PI-rtheta));      
    }
    
    //printf("nornalised=%f\n",norm);

                return  retval;

        }*/

 

        int need_to_move(Vector2D<int> target)
        {
                
                Vector2D<int> delta;
                delta.x = target.x - HP(botID).x;
                delta.y = target.y - HP(botID).y;

                if( delta.x>tolerance || delta.y>tolerance )
                        return 1;
                else
                        return 0;
        }
  Vector2D<int> cooperateWith(Vector2D<int> target, int attackBot)
  {
    
    Vector2D<int> attackBotPos = HP(attackBot);
    //first of  all the x coordinate of the target should be less than that of attk bot
    if( ForwardX(target.x) > ForwardX(attackBotPos.x) ) target.x = attackBotPos.x;
    Vector2D<int> delta;
    delta.x = target.x - attackBotPos.x;
    delta.y = target.y - attackBotPos.y;
    int dist = (int) Vector2D<int>::dist(target,attackBotPos);
    //printf("dist = %d BOTRADIUS =%f\n", dist,BOT_RADIUS );
    if(dist>3.7*BOT_RADIUS) return target;
    //else
    Vector2D<int> newdelta(delta),origin(0,0);
    //doing log search for good point, 
    //assert : newdelta != origin
    if(newdelta == origin)
    {
      newdelta.x = -attackBotPos.x + BP.x;
      newdelta.y = -attackBotPos.y + BP.y;
    }
    int count = 0 ;
    while(dist<=4.3*BOT_RADIUS)
    { 
      newdelta.x = newdelta.x*2 ;
      newdelta.y = newdelta.y*2 ; //log search kinda thing
      dist = Vector2D<int>::dist(newdelta,origin);
      //printf("count %d, newdelta.x = %d dist = %f\n",count,newdelta.x,dist);
    }
    target = attackBotPos + newdelta ;
    //don't return now , for all we now the bot might go out of bounds!!
    //retain non conflicting coordinates
    
    int px = ( target.x >= HALF_FIELD_MAXX*(REDUCE_FIELD)  );
	int nx = ( target.x <= -HALF_FIELD_MAXX*(REDUCE_FIELD)  );
	int py = ( target.y >= HALF_FIELD_MAXY*(REDUCE_FIELD)  );
	int ny = ( target.y <= -HALF_FIELD_MAXY*(REDUCE_FIELD)  );

    if(px) target.x = HALF_FIELD_MAXX*(REDUCE_FIELD);
    else if(nx) target.x = -HALF_FIELD_MAXX*(REDUCE_FIELD);
    if(py) target.y = HALF_FIELD_MAXY*(REDUCE_FIELD);
    else if(ny) target.y = -HALF_FIELD_MAXY*(REDUCE_FIELD);
    
    // now we are definately in the bounds return 
    return target;
  }
  
  int getattackbotID()
  {
    int minv = 0;
          float angle_difference = firaNormalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)- state->homeAngle[0]);
      int minwt = Vector2D<int>::dist(state->homePos[0],state->ballPos) + angle_difference * ANGLE_TO_DIST;
          
          for (int it = 0; it < HomeTeam::SIZE  ; ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = Vector2D<int>::dist(state->homePos[it],state->ballPos);
        float botballangle = normalizeAngle(Vector2D<int>::angle(state->ballPos, state->homePos[it]));
        //TODO might require normalization
        float botball_orientation_diff = MIN(fabs(botballangle-state->homeAngle[it]),fabs(botballangle-(state->homeAngle[it]+PI)));
        float finalOrientationDiff = normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)-botballangle);
//              angle_difference =  fabs(firaNormalizeAngle(state->homeAngle[*it]-normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos))))+ fabs(firaNormalizeAngle((Vector2D<int>::angle(state->homePos[*it],Vector2D<int>(OPP_GOAL_X, 0)))));
      angle_difference = botball_orientation_diff + finalOrientationDiff;
  //float x_diff = ForwardX(state->ballPos.x)-ForwardX(state->homePos.x);
      float weight;
      weight = dis_from_ball + ANGLE_TO_DIST * angle_difference;
      //if(*it == botID)
        //    weight -= HYSTERESIS;
          if(weight < minwt)
          {
            minwt = dis_from_ball ;
            minv = it;
          }
        }
        //Util::Logger::toStdOut("Selected bot %d\n", minv);
        return minv;
  }

	void execute(const Param& tParam)
    {
    //printf("Backup BotID: %d\n",botID);
    /* assert Added by Soumyadeep */
   // assert(state->oppNum > 2);
    //int attackBot = getattackbotID();
    //int attackBot = tParam.BackupP.BotID;
    Vector2D<int> target = harmonic();
    //target = cooperateWith(target , attackBot);
	SkillSet::addCircle(target.x, target.y, 50, 0x00);
                float lookat = looking_angle(target);
                sID = SkillSet::GoToPoint;
        sParam.GoToPointP.align = false;
        sParam.GoToPointP.finalslope = lookat;
        sParam.GoToPointP.x = target.x;
        sParam.GoToPointP.y = target.y;
        printf(" ( %d, %d )\n",target.x,target.y);
        skillSet->executeSkill(sID, sParam);
        }

  };//class tBackup
}//namespace Strategy

//tBackup.hpp - Sanyam
#endif // TTBackup_HPP