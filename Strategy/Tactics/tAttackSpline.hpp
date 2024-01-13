#ifndef TTATTACKSPLINE_HPP
#define TTATTACKSPLINE_HPP

#include <list>
#include "comdef.h"
#include "tactic.h"
#include "skillSet.h"
#include "beliefState.h"
#include "logger.h"
#include "config.h"
#include "intersection.hpp"
#include <fstream>
#define ANGLE_TO_DIST 0
namespace Strategy
{
  class TAttackSpline : public Tactic
  {
    static const int offset = 400;
    // Corner Case: wall, ball, bot in line
    float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
  int splin;
  int sCount;
  
  public:
    TAttackSpline(const BeliefState* state, int botID) :
      Tactic(Tactic::AttackSpline, state, botID)
    {
    sCount = 0 ;
    splin = 0;
      iState = APPROACHING;
      for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      hasAchievedOffset = 0;
    } // TAttack

    ~TAttackSpline()
    { } // ~TAttack
   enum InternalState
    {
      APPROACHING,
      SPINNING_CCW ,
      SPINNING_CW,
      ATTACKING,
      CLOSE_TO_BALL,
      STUCK
    } iState;
  
    int hasAchievedOffset;
    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
      float angle_difference = firaNormalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)- state->homeAngle[*(freeBots.begin())]);
      int minwt = Vector2D<int>::dist(state->homePos[*(freeBots.begin())],state->ballPos) + angle_difference * ANGLE_TO_DIST;
		if (splin && prevID > 0) {
			return prevID;
		}
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = Vector2D<int>::dist(state->homePos[*it],state->ballPos);
        float botballangle = normalizeAngle(Vector2D<int>::angle(state->ballPos, state->homePos[*it]));
        //TODO might require normalization
        float botball_orientation_diff = MIN(fabs(botballangle-state->homeAngle[*it]),fabs(botballangle-(state->homeAngle[*it]+PI)));
        float finalOrientationDiff = normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)-botballangle);
//    angle_difference =  fabs(firaNormalizeAngle(state->homeAngle[*it]-normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos))))+ fabs(firaNormalizeAngle((Vector2D<int>::angle(state->homePos[*it],Vector2D<int>(OPP_GOAL_X, 0)))));
      angle_difference = botball_orientation_diff + finalOrientationDiff;
  //float x_diff = ForwardX(state->ballPos.x)-ForwardX(state->homePos.x);
        float weight;
    //printf("%d >>>>>>>>>> %f , %f\n", *it,dis_from_ball,angle_difference);
        weight = dis_from_ball + ANGLE_TO_DIST * angle_difference;
        if(*it == prevID)
          weight -= HYSTERESIS;
        if(weight < minwt)
        {
          minwt = dis_from_ball ;
          minv = *it;
        }
      }
      //Util::Logger::toStdOut("Selected bot %d\n", minv);
	
      //assert(tParam=0);
      return minv;

    } // chooseBestBot
/////////////////////////////////////////

 bool pointxInField(Vector2D<int> final)
 {
      // checks if the point is in the field 
      if((final.x < HALF_FIELD_MAXX - (BALL_AT_CORNER_THRESH) && final.x > -HALF_FIELD_MAXX + (BALL_AT_CORNER_THRESH)))
      {
        if((final.y < HALF_FIELD_MAXY - BALL_AT_CORNER_THRESH && final.y > -HALF_FIELD_MAXY + BALL_AT_CORNER_THRESH))
        {
          return true;
        }
        else return false;
      }
      else  return false;
 }
//***********
 int opponentProbableGoalkeeper(){
// returns the id of opponent goal keeper 
    int distance=HALF_FIELD_MAXX/2;
    int id=-1;
    for(int i=0; i <5 ; i++){
    if(HALF_FIELD_MAXX-ForwardX(state->awayPos[i].x) < distance){
      if(state->awayPos[i].y < OPP_GOAL_MAXY + 2*BOT_RADIUS && state->awayPos[i].y > OPP_GOAL_MINY -2*BOT_RADIUS){ 
          distance=HALF_FIELD_MAXX-ForwardX(state->awayPos[i].x);
          id=i;
      }
   }
    } 
  return id;
  }
  
 Vector2D<int> decidedGoalPoint(){
   int id=opponentProbableGoalkeeper();
   Vector2D<int> pointToAttack;
   if(id==-1){
    pointToAttack.x=HALF_FIELD_MAXX-GOAL_DEPTH;
    pointToAttack.y=0;
    return pointToAttack;
   }
   Vector2D<int> pointOfAttack(HALF_FIELD_MAXX-GOAL_DEPTH, 0);
   float errorAngle=0.08726*2;
   float angleGoalMax_y, angleGoalMin_y;
   Vector2D<int> goal_maxy(HALF_FIELD_MAXX-GOAL_DEPTH, OPP_GOAL_MAXY);
   Vector2D<int> goal_miny(HALF_FIELD_MAXX-GOAL_DEPTH, OPP_GOAL_MINY);
   angleGoalMax_y=Vector2D<int>::angle(state->ballPos , goal_maxy)-errorAngle;
   angleGoalMin_y=Vector2D<int>::angle(state->ballPos, goal_miny)+errorAngle;
   if(state->awayPos[id].x > 0.6*OPP_GOAL_MAXY){
        pointOfAttack.y=angleGoalMin_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
        return pointOfAttack;
   }
   if(state->awayPos[id].x < 0.6*OPP_GOAL_MINY){
      pointOfAttack.y=angleGoalMax_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
      return pointOfAttack;
   }
   if(state->awayPos[id].y >=0 && state->awayVel[id].y >=0 ){
//  return angleGoalMin_y;
      pointOfAttack.y=angleGoalMin_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
      return pointOfAttack;
    }
    else if(state->awayPos[id].y <0 && state->awayVel[id].y <0){
    //return angleGoalMax_y;
        pointOfAttack.y=angleGoalMax_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
        return pointOfAttack;
    }
    return pointOfAttack; 
 }
  
 void shoot(){
    Vector2D<int> targetGoalPoint=decidedGoalPoint();
  Vector2D<int> ballPredictedPos;
  Vector2D<float> ballTransformedVel;
  float factor = 0.00005;
  float botBallAngle=Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
  float botBallDist=Vector2D<int>::dist(state->homePos[botID], state->ballPos);
  float ballGoalPointAngle=Vector2D<int>::angle(ballPredictedPos, targetGoalPoint);
  int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);

    ballPredictedPos.x = state->ballPos.x ;//+ (int)ballBotDist * factor * avgBallVel.x;
    ballPredictedPos.y = state->ballPos.y ;//+ (int)ballBotDist * factor * avgBallVel.y;
    sID=SkillSet::GoToPoint;
    sParam.GoToPointP.x=ballPredictedPos.x;
    sParam.GoToPointP.y=ballPredictedPos.y;
    sParam.GoToPointP.align=true;
    sParam.GoToPointP.finalslope=ballGoalPointAngle;
    sParam.GoToPointP.finalVelocity=MAX_BOT_SPEED;
    skillSet->executeSkill(sID,sParam);
    return;
 }

 int chooseOppReceiver()
 {
  int id=-1;
  for(int i=0;i<5;i++)
  {
    if(state->awayPos[i].x<-HALF_FIELD_MAXX+GOAL_DEPTH+DBOX_WIDTH+5*BOT_RADIUS && abs(state->awayPos[i].y)<OUR_GOAL_MAXY) return i;
  }
  return id;
 }

 bool isBallInDBox()
 {
  if((abs(state->ballPos.y)<OUR_GOAL_MAXY+ 5*BOT_RADIUS)&&(state->ballPos.x < -HALF_FIELD_MAXX+GOAL_DEPTH+DBOX_WIDTH)) 
     return true;

      return false;
 }
/////////////////////////////////////////
   
 void execute(const Param& tParam)
    { 
     printf("Attack BotID: %d\n",botID);
    
	float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
    float botBallAngle = normalizeAngle(Vector2D<int>::angle(state->homePos[botID] , state->ballPos));
    float ballGoalAngle = normalizeAngle(Vector2D<int>::angle(state->ballPos, Vector2D<int>(ForwardX(HALF_FIELD_MAXX), 0)));
	float diff = fabs(normalizeAngle(botBallAngle - ballGoalAngle));
  switch(iState)
  {        
  case APPROACHING:
  { 
	// SPINNIGN IN APPROACH
	 if(dist < BOT_BALL_THRESH)
     {
     if(fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 9 && fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 9)
       {
        if(state->ballPos.y < 0)
          iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;
    else
       iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;
     break ;
       } 
     }
	//*********************
    if(ForwardX(state->homePos[botID].x) < ForwardX(state->ballPos.x) /*&& diff < PI/8 */&& abs(state->homePos[botID].x - state->ballPos.x) > 0.5*BOT_BALL_THRESH)
    {
     if(dist < 2*BOT_BALL_THRESH)
     {
       iState = CLOSE_TO_BALL ;
       splin = 0 ; 
       break ;
     }
    } 
    if(	ForwardX(state->ballPos.x) < (-HALF_FIELD_MAXX / 2  ))
    { 
    sID=SkillSet::GoToPoint;
    cout<<"In DBox"<<endl;
	Vector2D<int>Goal(OPP_GOAL_X,0);
      sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX / 2  ) ;
      sParam.GoToPointP.y = -1*SGN(state->ballPos.y)*( OUR_GOAL_MAXY  );
	  sParam.GoToPointP.finalslope = Vector2D<int>::angle(Goal,state->homePos[botID]);
	  sParam.GoToPointP.align = true ;
      splin = 0 ;    
        skillSet->executeSkill(sID, sParam);
        break;
    } 
    /*
	if(state->ourBotNearestToBall!=botID && Vector2D<int>::dist(state->ballPos , state->homePos[state->ourBotNearestToBall]) < 2*BOT_BALL_THRESH)
	{ 
		cout<<"stopping"<<endl; 
	  	sID = SkillSet::Stop ;
		splin = 0 ;
		skillSet->executeSkill(sID , sParam) ;
		break ;
	}
	*/
/*    if(abs(abs(state->ballPos.y) - HALF_FIELD_MAXY) < 2*BOT_RADIUS || abs(abs(state->ballPos.x) - (HALF_FIELD_MAXX - GOAL_DEPTH - BOT_RADIUS*1.2)) < 2*BOT_RADIUS)
    {
    cout<<"Local Avoidance"<<endl ;
    sID = SkillSet::GoToPoint ;
    if(state->ballPos.x > state->homePos[botID].x)
    {
       sParam.GoToPointP.x = state->ballPos.x + BOT_BALL_THRESH ;   // so that it doesn't stops 
       sParam.GoToPointP.y = state->ballPos.y ;
       skillSet->executeSkill(sID , sParam );
       break ;
      }
    }*/
//    if(isBallInDBox()==true)
//    {
//      sParam.GoToPointP.x =  -HALF_FIELD_MAXX+GOAL_DEPTH+DBOX_WIDTH+BOT_RADIUS ;
//      sParam.GoToPointP.y =   SGN(state->ballPos.y)*(OUR_GOAL_MAXY+BOT_RADIUS);
//      if(Vector2D<int>::dist(state->homePos[botID],state->homePos[0])>2*BOT_BALL_THRESH)
//      { int id=chooseOppReceiver();

//      // required only in the game 
//      /*
//      if(state->ballPos.x<-HALF_FIELD_MAXX+GOAL_DEPTH+2*BOT_RADIUS && state->awayPos[id].x>-HALF_FIELD_MAXX+GOAL_DEPTH+BOT_RADIUS)
//      {
//        if(id!=-1) 
//        {
//          sParam.GoToPointP.x = state->awayPos[id].x-1.2*BOT_RADIUS ;
//          sParam.GoToPointP.y= state->awayPos[id].y;
//        }
//      }
//      */    
//    }
//       splin = 0 ;    
//    skillSet->executeSkill(sID, sParam);
//    break;
//    }

    // write the code for local avoidance also :: using CP in spline 
             cout<<"APPROACHING"<<endl ; 
          cout << "spline here :: " << splin << std::endl <<std::endl;
      sID = SkillSet::SplineInterceptBall;
    //  cout << "here" << endl;
      sParam.SplineInterceptBallP.vl = 0;
      sParam.SplineInterceptBallP.vr = 0;
      sParam.SplineInterceptBallP.velGiven = 1;
      sParam.SplineInterceptBallP.ballVelX = state->ballVel.x;
      sParam.SplineInterceptBallP.ballVelY = state->ballVel.y;
     if(splin == 0){
		splin = 1;
        sParam.SplineInterceptBallP.initTraj = 1;
      } else{
      splin = 1;
        sParam.SplineInterceptBallP.initTraj = 0;
      }
   //   if (dist < 500)
      sParam.SplineInterceptBallP.changeSpline = true;
 //     else
  //      sParam.SplineInterceptBallP.changeSpline = true;
    //  cout << "here " << endl;
      skillSet->executeSkill(sID, sParam);
      splin  = 1; 
      break;
    }

  case SPINNING_CW:
  {
    splin = 0;
     cout<<"SPINNING_CW"<<endl;
     if(dist>1.2*BOT_BALL_THRESH)
     {
		 iState = APPROACHING;
		 return;
     }
     // shoot();
     // break;
     sID = SkillSet::Spin;
     if(FIELD_IS_INVERTED == false)
		sParam.SpinP.radPerSec = (0.5*MAX_BOT_OMEGA);
     else
		sParam.SpinP.radPerSec = -(0.5*MAX_BOT_OMEGA);
     skillSet->executeSkill(sID, sParam);
    break;    
     }
  case SPINNING_CCW:
  {
  splin = 0;
    cout<<"SPINNING_CCW"<<endl;
    if(dist>1.2*BOT_BALL_THRESH)
    {
		iState = APPROACHING;
		return;
    }
         // shoot();
     // break;
    sID = SkillSet::Spin;
    if(FIELD_IS_INVERTED == false)
		sParam.SpinP.radPerSec = -(0.5*MAX_BOT_OMEGA);
    else
		sParam.SpinP.radPerSec = (0.5*MAX_BOT_OMEGA);
             
    skillSet->executeSkill(sID, sParam);
    break;
  }

  case CLOSE_TO_BALL:
  {
  splin = 0;
  
   if(dist < BOT_BALL_THRESH)
     {
     if(fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 9 && fabs(normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 9)
       {
        if(state->ballPos.y < 0)
          iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;
    else
       iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;
     break ;
       } 
     }
	if(dist > 3*BOT_BALL_THRESH)
	{
		iState = APPROACHING ;
		break ;
	}
	if(ForwardX(state->homePos[botID].x) > ForwardX(state->ballPos.x - 0.5*BOT_BALL_THRESH))
	{
		iState = APPROACHING ; 
		break ;
	}     
          /* Ball is with bot. So go to goal */
      sID = SkillSet::GoToPoint;
      int desty = 0;
      Vector2D<int> GoalMidPoint  (OPP_GOAL_X,0);
      Vector2D<int> GoalLeftPoint (OPP_GOAL_X,OPP_GOAL_MINY);
      Vector2D<int> GoalRightPoint(OPP_GOAL_X,OPP_GOAL_MAXY);
      float angleofBot = state->homeAngle[botID];
      if(angleofBot > (3 * PI)/4) angleofBot  = (3 * PI)/4;
      if(angleofBot < -(3 * PI)/4) angleofBot = -(3 * PI)/4;
      float R =  (OPP_GOAL_X - state->homePos[botID].x) / ( cos(angleofBot) );
      float destY = state->homePos[botID].y + R * sin(angleofBot);
      if(destY < OPP_GOAL_MINY + 200) destY = OPP_GOAL_MINY + 200;
      if(destY > OPP_GOAL_MAXY - 200) destY = OPP_GOAL_MAXY - 200;
      /*float angleWithGoal  =  Vector2D<int>::angle(state->homePos[botID],GoalMidPoint);
      float leftGoalAngle  =  Vector2D<int>::angle(GoalLeftPoint,state->homePos[botID]);
      float rightGoalAngle =  Vector2D<int>::angle(state->homePos[botID],GoalRightPoint);
	  */      
      // **************  YAHAN PE SPLINE ******************************************************* 
      cout<<":::::::::::::::    "<<dist<<" "<<1.5*BOT_BALL_THRESH<<endl;
    if (dist > 1.5*BOT_BALL_THRESH){// && diff < PI/8) {
        cout<<"CLOSE_TO_BALL :: Something else"<<endl;
        sParam.GoToPointP.align = false;
        sParam.GoToPointP.x = state->ballPos.x;// + 0.016*state->ballVel.x;
        sParam.GoToPointP.y = state->ballPos.y;// + 0.016*state->ballVel.y;
        sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
    }
    else {
         cout<<"CLOSE_TO_BALL :: DRAG_TO_GOAL"<<endl;
         sParam.GoToPointP.align = false;
         sParam.GoToPointP.x = OPP_GOAL_X;
         sParam.GoToPointP.y = 0 ; //destY;
      if(abs(state->ballPos.y)>HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[botID].y)>HALF_FIELD_MAXY-1.7 && botID==state->ourBotNearestToBall)
      {
		sParam.GoToPointP.finalslope = Vector2D<int>::angle(state->ballPos,state->homePos[botID]);
      }
      else if(abs(state->ballPos.y)>HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[botID].y)>HALF_FIELD_MAXY-1.7*BOT_RADIUS && botID!=state->ourBotNearestToBall)
	  {
		sParam.GoToPointP.finalslope = Vector2D<int>::angle(state->homePos[state->oppBotNearestToBall],state->homePos[botID]);
      }
      else
      {
		sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
      }    
    }
	sParam.GoToPointP.increaseSpeed = 1;
	skillSet->executeSkill(sID, sParam);           
	break ;
  }        
      }
    }  
      
  }; // class TAttack
} // namespace Strategy


#endif // TTCharge_HPP