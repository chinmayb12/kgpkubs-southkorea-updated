#include <sys/time.h>
#include "comdef.h"
#include "kalman.h"
#include "cs.hpp"
#include "beliefState.h"
#include "messages_robocup_ssl_detection.pb.h"
#include "common.pb.h"
#include "packet.pb.h"
#include "replacement.pb.h"
#include "rbCommDef.h"
#include "config.h"
#include "logger.h"
#include <limits>
#include <set>
#include <iostream>
#include <fstream>
#include "skillSet.h"

#define SGFILTER_SIZE 4
#define SGFILTER_DERIV 1
#define SGFILTER_ORDER 2

#define DEBUG_KALMAN 1
const int botcenterbluecenterdist =75; //Ankit: for botCentreTransform
bool isIndeterminate(const float pV)
{
    volatile float d = pV;
    return (d != d);
} 
inline void linearTransform(int &x, int &y, float &newangle)
{
  double tempx, tempy;
  tempx = (y*(-HALF_FIELD_MAXX))/(2050.0);
  tempy = ((x + 3025.0/2)*(HALF_FIELD_MAXY))/(3025.0/2.0);
  x = (int)tempx;
  y = (int)tempy;
  newangle = normalizeAngle(newangle+PI/2);
}

inline void botcenterTransform(int &x, int &y, float &newangle)//ssl vision detects blue center as bot center
{
  /// Commented. Not needed currently.
  // x=x-botcenterbluecenterdist*cos(newangle+3*PI/4);
  //y=y-botcenterbluecenterdist*sin(newangle+3*PI/4);
}
bool isInfinite(const float pV)
{
    return (fabs(pV) == std::numeric_limits<float>::infinity());
}
void checkValid(float &x, float resetVal) {
  if(isIndeterminate(x) || isInfinite(x)) {
    assert(0);
    x =resetVal;
  }
}
void checkValidX(float &x, float &slope, float resetVal) {
  if(x>=-2*HALF_FIELD_MAXX && x<=2*HALF_FIELD_MAXX)return;
  x = resetVal;
  slope = 0;
  checkValid(x, resetVal);
}
void checkValidY(float &y, float &slope, float resetVal) {
  if(y>=-2*HALF_FIELD_MAXY && y<=2*HALF_FIELD_MAXY)return;
  y = resetVal;
  slope = 0;
  checkValid(y, resetVal);
}
void checkValidA(float &a, float &slope, float resetVal) {
  if(a>=-2*PI && a<=2*PI)return;
  a = resetVal;
  slope = 0;
  checkValid(a, resetVal);
}
namespace Strategy
{
#ifdef VISION_COMM
  Kalman::Kalman()
  {
/*	KF = KalmanFilter(4, 2, 0);
	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
	measurement = Mat_<float>(2,1); measurement.setTo(Scalar(0));
	 
	KF.statePre.at<float>(0) = 0;
	KF.statePre.at<float>(1) = 0;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
	setIdentity(KF.measurementNoiseCov, Scalar::all(10));
	setIdentity(KF.errorCovPost, Scalar::all(.1));*/
	//cv::KalmanFilter kf(3, 2, 0);

    kalmanlog = fopen("kalman.log", "w");
    t.start();
    mutex = new Util::CS();

    for(int id = 0; id < HomeTeam::SIZE; ++id)
    {
      homePose[id].x     = 2*HALF_FIELD_MAXX + BOT_RADIUS*4;
      homePose[id].y     = 2*HALF_FIELD_MAXY + BOT_RADIUS*4;
      homeVelocity[id].x = 0;
      homeVelocity[id].y = 0;
	    homeVlVr[id].x = 0;
      homeVlVr[id].y = 0;
      homePosK[id].x     = 1.0;
      homePosK[id].y     = 1.0;
      homeAngle[id]      = 0;
      homeOmega[id]      = 0;
      homeAngleK[id]     = 1;
    }

    for(int id = 0; id < AwayTeam::SIZE; id++)
    {
      awayPose[id].x     = 2*HALF_FIELD_MAXX + BOT_RADIUS*4;
      awayPose[id].y     = 2*HALF_FIELD_MAXY + BOT_RADIUS*4;
      awayVelocity[id].x = 0;
      awayVelocity[id].y = 0;
      awayPosK[id].x     = 1;
      awayPosK[id].y     = 1;
      awayAngle[id]      = 0;
      awayOmega[id]      = 0;
      awayAngleK[id]     = 1;
    }

    ballPose.x     = 0;
    ballPose.y     = 0;
    ballVelocity.x = 0;
    ballVelocity.y = 0;
    ballPosK.x     = 1;
    ballPosK.y     = 1;
	
	 for(int i = 0 ; i<=5;i++){
		 QueueVel.push_back(Vector2D<float>(0,0));
		 QueuePos.push_back(Vector2D<float>(0,0));
		}
	 
	  myfileX.open ("/home/robocup/ballDataX.txt");
	  myfileY.open ("/home/robocup/ballDataY.txt");
	  
	for (int i = 0; i < MAX_BS_Q; i++) {
        bsQ.push(std::make_pair(BeliefState(), 0));
    }
	for(int i = 1 ; i<5;i++)
		ballPosQueue.push_back(Vector2D<float>(0,0));
	
/*	for(int i = 0; i < 2*SGFILTER_SIZE + 1 ;i++){
		ballPosX.push_back(0);
		ballPosTimeX.push_back(0.016);
		ballPosY.push_back(0);
		ballPosTimeY.push_back(0.016);
	}*/
  }

  Kalman::~Kalman()
  {
	//myfile.close();
    fclose(kalmanlog);
  }
	int Kalman::getClosestBotID(int x, int y, float angle, std::set<int> &uniqueBotIDs)
	{		
		int closest = -1;
		float closestCost = 9999999999.0;
    
		for (int botID = 0; botID < AwayTeam::SIZE; ++botID)
    {
			float cost = (x - awayPose[botID].x)*(x - awayPose[botID].x);
			cost += (y - awayPose[botID].y)*(y - awayPose[botID].y);
			cost += (angle - awayAngle[botID])*(angle - awayAngle[botID]);
			if(cost < closestCost && uniqueBotIDs.find(botID) == uniqueBotIDs.end()) {
				closest = botID;
				closestCost = cost;
			}				
		}
		return closest; //returns -1 if no suitable bot present.
	}
  static float nearestAngle(float angle, float reference)
  {
    float del = fabs(angle-reference);
    float res = angle;
    if(del > fabs(angle-reference+2*PI))
    {
      del = fabs(angle-reference+2*PI);
      res = angle+2*PI;
    }
    if(del > fabs(angle-reference-2*PI))
    {
      res = angle-2*PI;
    }
    return res;
  }

  Vector2D<float> Kalman::calcBotVelocity(double delX, double delY , double newTheta, double oldTheta, float timeMs) {
 //   strategyToRealConversion(p1);
  //  strategyToRealConversion(p2);
	float vl,vr;
    double delTheta = normalizeAngle(newTheta - oldTheta); // assuming |delTheta| < PI, which is safe to assume
	// for ~ 16 ms of rotation at any speed (even 120,-120?).
    assert(timeMs > 0);
    // w * timeMs = delTheta
    double w = delTheta / (timeMs);// * 0.001);

    if (delTheta < 1e-2 && delTheta > -1e-2) {  // be realistic
	
	//cout << "here" << endl;final_vl
        // bot should be headed straight, but again confusion
        // so taking projection of (delX, delY) along (cos(theta), sin(theta)) as displacement.
        double dispLength = delX*cos(oldTheta) + delY*sin(oldTheta);
        vl = dispLength / (timeMs);// * 0.001);
		assert(ticksToCmS > 0);
        vl = vl / ticksToCmS;
        vr = vl;
		Vector2D<float> v(vl,vr);
        return v;
    }
    // we calculate 2 rho's, based on delX and delY, and take average
  // whatif newTheta == oldTheta 
    
	double rho1 = delX / (sin(newTheta) - sin(oldTheta));
    double rho2 = -delY / (cos(newTheta)- cos(oldTheta));
	//cout<<" Rhos :: "<<newTheta<<"  "<<oldTheta<< " " << delTheta << endl ;
    // try harmonic mean?
//    double rho = (rho1 + rho2) / 2;
	assert(ticksToCmS > 0);
	double rho;
	if (rho1 + rho2 < 1e-2 && rho1 + rho2 > -1e-2) { // cant do harmonic mean in this case
		rho = (rho1 + rho2)/2;
	} else {
		rho = 2*rho1*rho2 / (rho1 + rho2);
	}
    vl = w * (rho - d/2.0) / ticksToCmS;
    vr = w * (rho + d/2.0) / ticksToCmS;
	//cout<<"Calculated velocity :: " << vl << "   " << vr << endl;
	assert(vl == vl && vr == vr);
	Vector2D<float> v(vl,vr);
    return v;
  }
	void  Kalman::updateField(SSL_GeometryData& geometry)
	{
		//cout << "here" << endl;
		SSL_GeometryFieldSize fiel;
		fiel = geometry.field();
	//	cout<< geometry.field().test_num()<<"<><><><><><><><<><><\n"<<endl;
	}
  void Kalman::addInfo(SSL_DetectionFrame &detection)
  {
    static double minDelTime = 999999;
    static double lastTime = 0;
    mutex->enter();
    //double timeCapture           = detection.t_capture();
    int ballsNum                 = detection.balls_size();
    blueNum                      = MIN(detection.robots_blue_size(), Simulator::BlueTeam::SIZE);
    yellowNum                    = MIN(detection.robots_yellow_size(), Simulator::YellowTeam::SIZE);

	//fprintf(kalmanlog, ">>>>>>>>>>>>>>>>Time: %f\n", timeCapture - lastTime);
    //double timeCapture      = detection.t_capture()/1000.0; //This is only valid if both the other side also uses the same reference time
    // neglecting the time for network delay. and wait time in race conditions 
    struct timeval tv;
    assert(gettimeofday(&tv, NULL) == 0);
    double timeCapture      = tv.tv_sec+tv.tv_usec/1000000.0;
    
    lastTime = timeCapture;
    std::set<int> uniqueBotIDs;
	
	double nowTime = detection.t_capture();

	//Adding Ball Info
    SSL_DetectionBall ball;
    if (ballsNum > 0)
    {
      ball = detection.balls(0);
	  
      int newx = ball.x() - CENTER_X;
      int newy = ball.y() - CENTER_Y;
      double delTime = timeCapture - ballLastUpdateTime;

      float garbage;
      #if GR_SIM_COMM || FIRASSL_COMM
      linearTransform(newx, newy, garbage);
	 // std::cout << "\n\n\nhere linear\n\n\n" << std::endl;
      #endif
      float  lastPoseX           = ballPose.x;
      ballPose.x                 = newx;
      float  lastPoseY           = ballPose.y;
      ballPose.y                 = newy;
      
	  float lastVelocityx        = ballVelocity.x;
	  float lastVelocityy        = ballVelocity.y;
	  prevBallVelocity.x        = ballVelocity.x;
	  prevBallVelocity.y        = ballVelocity.y;
	  ballVelocity.x             = (ballPose.x - lastPoseX) / delTime;
	  ballVelocity.y             = (ballPose.y - lastPoseY) / delTime;
	  
	  SgFilter sg;	  	 
	  vector<float> ballT;	
	  float ans = 0;	  
	  // for x direction
	  ballPosX.push_back(ballPose.x);
	  ballPosTimeX.push_back(delTime);  
	  
	  if( ballPosX.size() >= 2*SGFILTER_SIZE + 1){ 
		  ans = 0;			  
		  for(int i = 0 ; i < 2*SGFILTER_SIZE + 1 ; i++){
			  ans += ballPosTimeX[i];
			  ballT.push_back(ans);
		  }	
		  // getchar();
		  //myfileX << ballPose.x << "\t" <<  sg.smooth(ballT , ballPosX, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV) << endl;
		  ballVelocity.x =  sg.smooth(ballT , ballPosX, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV );	
		//  ballAcceleration.x =  sg.smooth(ballT , ballPosX, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV + 1);
		  ballPosX.erase(ballPosX.begin());
		  ballPosTimeX.erase(ballPosTimeX.begin());	 
	  }
	  ballT.clear();
	  // for y direction
	  ballPosY.push_back(ballPose.y);
	  ballPosTimeY.push_back(delTime);	
	  if( ballPosY.size() >= 2*SGFILTER_SIZE + 1){  	  
		  ans = 0;
		  for(int i = 0 ; i < 2*SGFILTER_SIZE + 1 ; i++){
			  ans += ballPosTimeY[i];
			  ballT.push_back(ans);
		  }
		  //myfileY << ballVelocity.y << "\t"  <<  sg.smooth(ballT , ballPosY, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV) << endl;
		  ballVelocity.y =  sg.smooth(ballT , ballPosY, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV );
		//  ballAcceleration.y =  sg.smooth(ballT , ballPosY, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV + 1);
		  ballPosY.erase(ballPosY.begin());
		  ballPosTimeY.erase(ballPosTimeY.begin());	
	  }
	  ballT.clear();
	  checkValidX(ballPose.x, ballVelocity.x, newx);
      checkValidY(ballPose.y, ballVelocity.y, newy);
      ballLastUpdateTime         = timeCapture;
    }

    if (HomeTeam::COLOR == Simulator::BLUE_TEAM)
    {
      // Blue robot info
      
      for (int i = 0; i < blueNum; ++i)
      {
        SSL_DetectionRobot robot = detection.robots_blue(i);
		
		//Home Blue Bot Info
        int                id    = HAL::BlueMarkerMap[robot.robot_id()];
        
        int newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;
        float newangle = robot.orientation();
//        printf("robot %d: %d %d %f\n", id, newx, newy, newangle);
        if(uniqueBotIDs.find(id) != uniqueBotIDs.end())
          continue;
        uniqueBotIDs.insert(id);
       // printf("newxy : %d %d\n", newx, newy);
        
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif
        double           delTime = timeCapture - homeLastUpdateTime[id];
		double timeMs = delTime ; //(nowTime - bsQ.front().second)*1000.0;
		if(timeMs <= 0){
			timeMs = 0.001;
		}
        static float lastnx = 0;
		float  lastPoseX        = homePose[id].x;
		float  lastPoseY         = homePose[id].y;
		float  lastAngle         = homeAngle[id];
		float lastVelocityx      = homeVelocity[id].x;
        float lastVelocityy      = homeVelocity[id].y;
        float lastAngularV       = homeOmega[id];
		homePose[id].x = newx;
		homePose[id].y = newy;
		homeAngle[id] = newangle;
		homeVelocity[id].x = (newx - lastPoseX)/delTime;
		homeVelocity[id].y = (newy - lastPoseY)/delTime;
        homeAcc[id].x            = (homeVelocity[id].x - lastVelocityx) / delTime;
		homeAcc[id].y           = (homeVelocity[id].y - lastVelocityy) / delTime;
        homeOmega[id]            = (homeAngle[id] - lastAngle) / delTime;
        homeAngularAcc[id]       = (homeOmega[id] - lastAngularV) / delTime;
		
        if(homeAngle[id] > PI)  
			homeAngle[id] -=2*PI;
        else if(homeAngle[id] <=-PI)
			homeAngle[id] += 2*PI;
        homeLastUpdateTime[id]   = timeCapture;
        
		//Adding vl,vr calculation from motion-simulation
		homeVlVr[id] = calcBotVelocity((newx - lastPoseX)/fieldXConvert , (newy - lastPoseY)/fieldXConvert, newangle, lastAngle, timeMs);
        checkValidX(homePose[id].x, homeVelocity[id].x, newx);
        checkValidY(homePose[id].y, homeVelocity[id].y, newy);
     //   checkValidA(homeAngle[id], homeOmega[id], newangle);
      }
      // Yellow  Away robot info
      uniqueBotIDs.clear();
      for (int i = 0; i < yellowNum; ++i)
      {
        
        SSL_DetectionRobot robot = detection.robots_yellow(i);
		int newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;
        float newangle = robot.orientation();
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif
        //int                id    = HAL::YellowMarkerMap[robot.robot_id()];				
				/* Arpit: Modifying code here to assuming that NO botid of opp team is known. Plz be careful!
				 * Randomly assigning IDs to each bot. if less no. of bots know, then 0...i ids will be populated, 
				 * i+1...4 ids will not be used. 
				 */
				int id_ = getClosestBotID(newx, newy, newangle, uniqueBotIDs);
				if(id_ == -1) //means all bots already populated
					continue;
				int id = HAL::YellowMarkerMap[id_];
				uniqueBotIDs.insert(id);
        //printf("ID = %d, loc = %d, %d\n", id, newx, newy);
        double           delTime = timeCapture - awayLastUpdateTime[id];
		float  lastPoseX         = awayPose[id].x;
		float  lastPoseY         = awayPose[id].y;
		float  lastAngle         = awayAngle[id];
		float lastVelocityx      = awayVelocity[id].x;
        float lastVelocityy      = awayVelocity[id].y;
        float lastAngularV       = awayOmega[id];
		awayPose[id].x = newx;
		awayPose[id].y = newy;
		awayAngle[id] = newangle;
		awayVelocity[id].x = (newx - lastPoseX)/delTime;
		awayVelocity[id].y = (newy - lastPoseY)/delTime;
        awayAcc[id].x            = (awayVelocity[id].x - lastVelocityx) / delTime;
		awayAcc[id].y           = (awayVelocity[id].y - lastVelocityy) / delTime;
        awayOmega[id]            = (awayAngle[id] - lastAngle) / delTime;
        awayAngularAcc[id]       = (awayOmega[id] - lastAngularV) / delTime;
		
        if(awayAngle[id] > PI)       
			awayAngle[id] -= 2*PI;
        else if(awayAngle[id] <=-PI) 
			awayAngle[id] += 2*PI;
        awayLastUpdateTime[id]   = timeCapture;
		
		//Adding vl,vr calculation from motion-simulation
		
        checkValidX(awayPose[id].x, awayVelocity[id].x, newx);
        checkValidY(awayPose[id].y, awayVelocity[id].y, newy);
        checkValidA(awayAngle[id], awayOmega[id], newangle);
        //printf("Awaybots %d: %f %f %f %f %lf\n", id, awayPose[id].x, awayPose[id].y, predictedPoseX, awayPosK[id].y, delTime);
      }
    }
    else
    {
      // Home Yellow robot info
      uniqueBotIDs.clear();
      for (int i = 0; i < yellowNum; ++i)
      {
        SSL_DetectionRobot robot = detection.robots_yellow(i);
        int id    = HAL::YellowMarkerMap[robot.robot_id()];
        if(uniqueBotIDs.find(id) != uniqueBotIDs.end())
          continue;
        uniqueBotIDs.insert(id);
        int newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;   
		float newangle;
		if(robot.has_orientation())
			newangle = robot.orientation();
		else
			newangle = 0;
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif
        double           delTime = timeCapture - homeLastUpdateTime[id];
	//	double timeMs = delTime ; //(nowTime - bsQ.front().second)*1000.0;
	//	if(timeMs <= 0){
	//		timeMs = 0.001;
	//	}		
		float  lastPoseX         = homePose[id].x;
		float  lastPoseY         = homePose[id].y;
		float  lastAngle         = homeAngle[id];
		float lastVelocityx      = homeVelocity[id].x;
        float lastVelocityy      = homeVelocity[id].y;
        float lastAngularV       = homeOmega[id];
		homePose[id].x = newx;
		homePose[id].y = newy;
		homeAngle[id] = newangle;
		homeVelocity[id].x = (newx - lastPoseX)/delTime;
		homeVelocity[id].y = (newy - lastPoseY)/delTime;
        homeAcc[id].x            = (homeVelocity[id].x - lastVelocityx) / delTime;
		homeAcc[id].y           = (homeVelocity[id].y - lastVelocityy) / delTime;
        homeOmega[id]            = (homeAngle[id] - lastAngle) / delTime;
        homeAngularAcc[id]       = (homeOmega[id] - lastAngularV) / delTime;
        
		if(homeAngle[id] > PI)
			homeAngle[id] -= 2*PI;
        else if(homeAngle[id] <=-PI) 
			homeAngle[id] += 2*PI;
        checkValidX(homePose[id].x, homeVelocity[id].x, newx);
        checkValidY(homePose[id].y, homeVelocity[id].y, newy);
        checkValidA(homeAngle[id], homeOmega[id], newangle);
        homeLastUpdateTime[id]   = timeCapture;
		
		//Adding vl,vr calculation from motion-simulation
		homeVlVr[id] = calcBotVelocity((newx - lastPoseX)/fieldXConvert, (newy - lastPoseY)/fieldXConvert, newangle, lastAngle, delTime);
		//cout << (newx - lastPoseX)/fieldXConvert << " " << (newy - lastPoseY)/fieldXConvert << " " << newangle << " " << lastAngle << " " << delTime << endl;
	    myfileX <<" homeVlVr[id] :: "<<homeVlVr[id].x<<"  "<<homeVlVr[id].y<<endl;
	  }
	  
      // Blue robot info
      uniqueBotIDs.clear();
      for (int i = 0; i < blueNum; ++i)
      {        
        SSL_DetectionRobot robot = detection.robots_blue(i);        
        int newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;
        float newangle = robot.orientation();
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif						
				/* Arpit: Modifying code here to assuming that NO botid of opp team is known. Plz be careful!
				 * Randomly assigning IDs to each bot. if less no. of bots know, then 0...i ids will be populated, 
				 * i+1...4 ids will not be used. 
				 */
		int id_ = getClosestBotID(newx, newy, newangle, uniqueBotIDs);
		if(id_ == -1) //means all bots already populated
				continue;
		int id = HAL::BlueMarkerMap[id_];
		uniqueBotIDs.insert(id);
        //printf("ID = %d, loc = %d, %d\n", id, newx, newy);
				
				
        double           delTime = timeCapture - awayLastUpdateTime[id];
		float  lastPoseX         = awayPose[id].x;
		float  lastPoseY         = awayPose[id].y;
		float  lastAngle         = awayAngle[id];
		float lastVelocityx      = awayVelocity[id].x;
        float lastVelocityy      = awayVelocity[id].y;
        float lastAngularV       = awayOmega[id];
		awayPose[id].x = newx;
		awayPose[id].y = newy;
		awayAngle[id] = newangle;
		awayVelocity[id].x = (newx - lastPoseX)/delTime;
		awayVelocity[id].y = (newy - lastPoseY)/delTime;
        awayAcc[id].x            = (awayVelocity[id].x - lastVelocityx) / delTime;
		awayAcc[id].y           = (awayVelocity[id].y - lastVelocityy) / delTime;
        awayOmega[id]            = (awayAngle[id] - lastAngle) / delTime;
        awayAngularAcc[id]       = (awayOmega[id] - lastAngularV) / delTime;
        if(awayAngle[id] > PI)        
			awayAngle[id] -= 2*PI;
        else if(awayAngle[id] <=-PI)  
			awayAngle[id] += 2*PI;
        checkValidX(awayPose[id].x, awayVelocity[id].x, newx);
        checkValidY(awayPose[id].y, awayVelocity[id].y, newy);
        checkValidA(awayAngle[id], awayOmega[id], newangle);
        
        awayLastUpdateTime[id]   = timeCapture;
        //printf("Awaybots %d: %f %f %f %f %lf\n", id, awayPose[id].x, awayPose[id].y, predictedPoseX, awayPosK[id].y, delTime);
      }
    }

	bsQ.pop();
	bsQ.push(std::make_pair(BeliefState(), nowTime));
   mutex->leave();
  }

  void Kalman::update(BeliefState& state)
  {
    mutex->enter();
    // TODO : Add delTime here and update the state state with respect to delTime
    struct timeval tv2;
    gettimeofday(&tv2, NULL);
    double timeNow      = tv2.tv_sec+tv2.tv_usec/1000000.0;
    state.currFrameNum++;
	
	/* new adds for ourNum and oppNum*/
    if (HomeTeam::COLOR == Simulator::BLUE_TEAM)
    {
      state.ourNum = blueNum;
      state.oppNum = yellowNum;
    }
    else if(HomeTeam::COLOR == Simulator::YELLOW_TEAM)
    {
      state.ourNum = yellowNum;
      state.oppNum = blueNum;
    }
    else
    {
      assert(HomeTeam::COLOR == Simulator::YELLOW_TEAM || HomeTeam::COLOR == Simulator::BLUE_TEAM);
    }
	
    for (int botID = 0; botID < HomeTeam::SIZE; ++botID)
    {
      double delTime = 0;
      state.homePos[botID]   = Vector2D<int>(homePose[botID].x, homePose[botID].y);/*Vector2D<int>(homePose[botID].x+homeVelocity[botID].x*delTime, 
      homePose[botID].y+homeVelocity[botID].y*delTime);*/
      printf("Home Pose: %d %d %d\n",botID,state.homePos[botID].x,state.homePos[botID].y);
	  state.homeAngle[botID] = homeAngle[botID];// + homeOmega[botID]*delTime;
      state.homeVel[botID]   = homeVelocity[botID];
	  state.homeVlVr[botID] = homeVlVr[botID];
	  // set the sent vl, vr from comm
	  {
		int vl, vr;		
		SkillSet::comm->getSentData(botID, vl, vr);
		state.homeSentVlVr[botID].x = vl;
		state.homeSentVlVr[botID].y = vr;
	  }	  
      state.homeOmega[botID] = homeOmega[botID];
      state.homeAcc[botID]   = homeAcc[botID];
      state.homeAngAcc[botID]= homeAngularAcc[botID];
    }
    //printf("updating %d %d\n", state.homePos[1].x, state.homePos[1].y);
    for (int botID = 0; botID < AwayTeam::SIZE; ++botID)
    {
      double delTime = 0;
      state.awayPos[botID]   = Vector2D<int>(awayPose[botID].x, awayPose[botID].y);
      state.awayAngle[botID] = awayAngle[botID];
      state.awayVel[botID]   = awayVelocity[botID];
      state.awayOmega[botID] = awayOmega[botID];
      state.awayAcc[botID]   = awayAcc[botID];
      state.awayAngAcc[botID]= awayAngularAcc[botID];
    }
      double delTime = 0;
	  
      state.ballPos = Vector2D<int>(ballPose.x + delTime*ballVelocity.x, ballPose.y + delTime*ballVelocity.y);
      state.ballVel = ballVelocity;
      state.ballAcc = ballAcceleration;
	  state.prevBallVel = prevBallVelocity;
	//  cout << ballAcceleration.x << " " << ballAcceleration.y << endl;
    //printf("Omega-> %lf\n",state.homeOmega[2]);

        /*for (int botID = 0; botID < HomeTeam::SIZE; ++botID)
    {
        if((v.readBuffer.botArr[HOME_TEAM][botID].center.x != 0) && (v.readBuffer.botArr[HOME_TEAM][botID].center.y != 0))
        {
      state.homePos[botID]   = Vector2D<int>(v.readBuffer.botArr[HOME_TEAM][botID].center.x,v.readBuffer.botArr[HOME_TEAM][botID].center.y);
      
      //state.homeAngle[botID] = homeAngle[botID];
          state.homeAngle[botID] = v.readBuffer.botArr[HOME_TEAM][botID].orientation;
        }
      //state.homeVel[botID]   = homeVelocity[botID];
      //state.homeOmega[botID] = homeOmega[botID];
                        //state.homeAcc[botID]   = homeAcc[botID];
                        //state.homeAngAcc[botID]= homeAngularAcc[botID];
    }
        for (int botID = 0; botID < AwayTeam::SIZE; ++botID)
    {
      state.awayPos[botID]   = Vector2D<int>(v.readBuffer.botArr[AWAY_TEAM][botID].center.x,v.readBuffer.botArr[AWAY_TEAM][botID].center.y);
      //state.homeAngle[botID] = homeAngle[botID];
      state.awayAngle[botID] = v.readBuffer.botArr[AWAY_TEAM][botID].orientation;
      //state.homeVel[botID]   = homeVelocity[botID];
      //state.homeOmega[botID] = homeOmega[botID];
                        //state.homeAcc[botID]   = homeAcc[botID];
                        //state.homeAngAcc[botID]= homeAngularAcc[botID];
    }
    state.ballPos = Vector2D<int>(v.readBuffer.ball.x,v.readBuffer.ball.y);
    printf("Ball pos %d,%d\n",state.ballPos.x,state.ballPos.y);
    printf("Aqua bot %d,%d\n",state.homePos[1].x,state.homePos[1].x);*/
    
    state.computeBallLocation();
    state.computeBallInDBox();
    //state.computeBallSide();
    state.computeBallInStrips();
    mutex->leave();
  }


#else
  Kalman::Kalman()
  {
/*	KF = KalmanFilter(4, 2, 0);
	KF.transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
	measurement = Mat_<float>(2,1); measurement.setTo(Scalar(0));
	 
	KF.statePre.at<float>(0) = 0;
	KF.statePre.at<float>(1) = 0;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
	setIdentity(KF.measurementNoiseCov, Scalar::all(10));
	setIdentity(KF.errorCovPost, Scalar::all(.1));*/
	//cv::KalmanFilter kf(3, 2, 0);

    kalmanlog = fopen("kalman.log", "w");
    t.start();
    mutex = new Util::CS();

    for(int id = 0; id < HomeTeam::SIZE; ++id)
    {
      homePose[id].x     = 2*HALF_FIELD_MAXX + BOT_RADIUS*4;
      homePose[id].y     = 2*HALF_FIELD_MAXY + BOT_RADIUS*4;
      homeVelocity[id].x = 0;
      homeVelocity[id].y = 0;
	    homeVlVr[id].x = 0;
      homeVlVr[id].y = 0;
      homePosK[id].x     = 1.0;
      homePosK[id].y     = 1.0;
      homeAngle[id]      = 0;
      homeOmega[id]      = 0;
      homeAngleK[id]     = 1;
    }

    for(int id = 0; id < AwayTeam::SIZE; id++)
    {
      awayPose[id].x     = 2*HALF_FIELD_MAXX + BOT_RADIUS*4;
      awayPose[id].y     = 2*HALF_FIELD_MAXY + BOT_RADIUS*4;
      awayVelocity[id].x = 0;
      awayVelocity[id].y = 0;
      awayPosK[id].x     = 1;
      awayPosK[id].y     = 1;
      awayAngle[id]      = 0;
      awayOmega[id]      = 0;
      awayAngleK[id]     = 1;
    }

    ballPose.x     = 0;
    ballPose.y     = 0;
    ballVelocity.x = 0;
    ballVelocity.y = 0;
    ballPosK.x     = 1;
    ballPosK.y     = 1;
	
	 for(int i = 0 ; i<=5;i++){
		 QueueVel.push_back(Vector2D<float>(0,0));
		 QueuePos.push_back(Vector2D<float>(0,0));
		}
	 
	  myfileX.open ("/home/robocup/ballDataX.txt");
	  myfileY.open ("/home/robocup/ballDataY.txt");
	  
	for (int i = 0; i < MAX_BS_Q; i++) {
        bsQ.push(std::make_pair(BeliefState(), 0));
    }
	for(int i = 1 ; i<5;i++)
		ballPosQueue.push_back(Vector2D<float>(0,0));
	
/*	for(int i = 0; i < 2*SGFILTER_SIZE + 1 ;i++){
		ballPosX.push_back(0);
		ballPosTimeX.push_back(0.016);
		ballPosY.push_back(0);
		ballPosTimeY.push_back(0.016);
	}*/
  }

  Kalman::~Kalman()
  {
	//myfile.close();
    fclose(kalmanlog);
  }
	int Kalman::getClosestBotID(int x, int y, float angle, std::set<int> &uniqueBotIDs)
	{		
		int closest = -1;
		float closestCost = 9999999999.0;
    
		for (int botID = 0; botID < AwayTeam::SIZE; ++botID)
    {
			float cost = (x - awayPose[botID].x)*(x - awayPose[botID].x);
			cost += (y - awayPose[botID].y)*(y - awayPose[botID].y);
			cost += (angle - awayAngle[botID])*(angle - awayAngle[botID]);
			if(cost < closestCost && uniqueBotIDs.find(botID) == uniqueBotIDs.end()) {
				closest = botID;
				closestCost = cost;
			}				
		}
		return closest; //returns -1 if no suitable bot present.
	}
  static float nearestAngle(float angle, float reference)
  {
    float del = fabs(angle-reference);
    float res = angle;
    if(del > fabs(angle-reference+2*PI))
    {
      del = fabs(angle-reference+2*PI);
      res = angle+2*PI;
    }
    if(del > fabs(angle-reference-2*PI))
    {
      res = angle-2*PI;
    }
    return res;
  }

  Vector2D<float> Kalman::calcBotVelocity(double delX, double delY , double newTheta, double oldTheta, float timeMs) {
 //   strategyToRealConversion(p1);
  //  strategyToRealConversion(p2);
	float vl,vr;
    double delTheta = normalizeAngle(newTheta - oldTheta); // assuming |delTheta| < PI, which is safe to assume
	// for ~ 16 ms of rotation at any speed (even 120,-120?).
    assert(timeMs > 0);
    // w * timeMs = delTheta
    double w = delTheta / (timeMs);// * 0.001);

    if (delTheta < 1e-2 && delTheta > -1e-2) {  // be realistic
	
	//cout << "here" << endl;final_vl
        // bot should be headed straight, but again confusion
        // so taking projection of (delX, delY) along (cos(theta), sin(theta)) as displacement.
        double dispLength = delX*cos(oldTheta) + delY*sin(oldTheta);
        vl = dispLength / (timeMs);// * 0.001);
		assert(ticksToCmS > 0);
        vl = vl / ticksToCmS;
        vr = vl;
		Vector2D<float> v(vl,vr);
        return v;
    }
    // we calculate 2 rho's, based on delX and delY, and take average
  // whatif newTheta == oldTheta 
    
	double rho1 = delX / (sin(newTheta) - sin(oldTheta));
    double rho2 = -delY / (cos(newTheta)- cos(oldTheta));
	//cout<<" Rhos :: "<<newTheta<<"  "<<oldTheta<< " " << delTheta << endl ;
    // try harmonic mean?
//    double rho = (rho1 + rho2) / 2;
	assert(ticksToCmS > 0);
	double rho;
	if (rho1 + rho2 < 1e-2 && rho1 + rho2 > -1e-2) { // cant do harmonic mean in this case
		rho = (rho1 + rho2)/2;
	} else {
		rho = 2*rho1*rho2 / (rho1 + rho2);
	}
    vl = w * (rho - d/2.0) / ticksToCmS;
    vr = w * (rho + d/2.0) / ticksToCmS;
	//cout<<"Calculated velocity :: " << vl << "   " << vr << endl;
	assert(vl == vl && vr == vr);
	Vector2D<float> v(vl,vr);
    return v;
  }

  	void  Kalman::updateField(fira_message::Field& geometry)
	{
		//cout << "here" << endl;
		// SSL_GeometryFieldSize fiel;
		// fiel = geometry.field();
	//	cout<< geometry.field().test_num()<<"<><><><><><><><<><><\n"<<endl;
	}
  void Kalman::addInfo(fira_message::Frame& frame)
  {
    static double minDelTime = 999999;
    static double lastTime = 0;
    mutex->enter();
    //double timeCapture           = detection.t_capture();
    int ballsNum                 = 1;
    blueNum                      = MIN(frame.robots_blue_size(), Simulator::BlueTeam::SIZE);
    yellowNum                    = MIN(frame.robots_yellow_size(), Simulator::YellowTeam::SIZE);

    std::cout<<"Bluenum "<<blueNum<<std::endl;
 
	//fprintf(kalmanlog, ">>>>>>>>>>>>>>>>Time: %f\n", timeCapture - lastTime);
    //double timeCapture      = detection.t_capture()/1000.0; //This is only valid if both the other side also uses the same reference time
    // neglecting the time for network delay. and wait time in race conditions 
    struct timeval tv;
    assert(gettimeofday(&tv, NULL) == 0);
    double timeCapture      = tv.tv_sec+tv.tv_usec/1000000.0;
    
    lastTime = timeCapture;
    std::set<int> uniqueBotIDs;
	
	double nowTime = 1;

	//Adding Ball Info
    fira_message::Ball ball;
    if (ballsNum > 0)
    {
      ball = frame.ball();

      int newx = ball.x() - CENTER_X;
      int newy = ball.y() - CENTER_Y;
      // std::cout<<"Ball "<<newx<<" "<<newy<<std::endl;
      double delTime = timeCapture - ballLastUpdateTime;

      float garbage;
      #if GR_SIM_COMM || FIRASSL_COMM
      linearTransform(newx, newy, garbage);
	 // std::cout << "\n\n\nhere linear\n\n\n" << std::endl;
      #endif
      float  lastPoseX           = ballPose.x;
      ballPose.x                 = newx;
      float  lastPoseY           = ballPose.y;
      ballPose.y                 = newy;
      
	  float lastVelocityx        = ballVelocity.x;
	  float lastVelocityy        = ballVelocity.y;
	  prevBallVelocity.x        = ballVelocity.x;
	  prevBallVelocity.y        = ballVelocity.y;
	  ballVelocity.x             = (ballPose.x - lastPoseX) / delTime;
	  ballVelocity.y             = (ballPose.y - lastPoseY) / delTime;
	  
	  SgFilter sg;	  	 
	  vector<float> ballT;	
	  float ans = 0;	  
	  // for x direction
	  ballPosX.push_back(ballPose.x);
	  ballPosTimeX.push_back(delTime);  
	  
	  if( ballPosX.size() >= 2*SGFILTER_SIZE + 1){ 
		  ans = 0;			  
		  for(int i = 0 ; i < 2*SGFILTER_SIZE + 1 ; i++){
			  ans += ballPosTimeX[i];
			  ballT.push_back(ans);
		  }	
		  // getchar();
		  //myfileX << ballPose.x << "\t" <<  sg.smooth(ballT , ballPosX, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV) << endl;
		  ballVelocity.x =  sg.smooth(ballT , ballPosX, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV );	
		//  ballAcceleration.x =  sg.smooth(ballT , ballPosX, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV + 1);
		  ballPosX.erase(ballPosX.begin());
		  ballPosTimeX.erase(ballPosTimeX.begin());	 
	  }
	  ballT.clear();
	  // for y direction
	  ballPosY.push_back(ballPose.y);
	  ballPosTimeY.push_back(delTime);	
	  if( ballPosY.size() >= 2*SGFILTER_SIZE + 1){  	  
		  ans = 0;
		  for(int i = 0 ; i < 2*SGFILTER_SIZE + 1 ; i++){
			  ans += ballPosTimeY[i];
			  ballT.push_back(ans);
		  }
		  //myfileY << ballVelocity.y << "\t"  <<  sg.smooth(ballT , ballPosY, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV) << endl;
		  ballVelocity.y =  sg.smooth(ballT , ballPosY, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV );
		//  ballAcceleration.y =  sg.smooth(ballT , ballPosY, 2*SGFILTER_SIZE + 1, SGFILTER_ORDER, SGFILTER_DERIV + 1);
		  ballPosY.erase(ballPosY.begin());
		  ballPosTimeY.erase(ballPosTimeY.begin());	
	  }
	  ballT.clear();
	  checkValidX(ballPose.x, ballVelocity.x, newx);
      checkValidY(ballPose.y, ballVelocity.y, newy);
      ballLastUpdateTime         = timeCapture;
    }

    std::cout<<HomeTeam::COLOR<<" "<<Simulator::BLUE_TEAM<<std::endl;
    if (HomeTeam::COLOR == Simulator::BLUE_TEAM)
    {
      // Blue robot info
      
      for (int i = 0; i < blueNum; ++i)
      {
        fira_message::Robot robot = frame.robots_blue(i);
		
		//Home Blue Bot Info
        int                id    = HAL::BlueMarkerMap[robot.robot_id()];
        
        std::cout<<"Robot"<<i<<" "<<robot.x()<<" "<<robot.y()<<std::endl;
        int newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;
        std::cout<<"Robot"<<i<<" "<<newx<<" "<<newy<<std::endl;
        float newangle = robot.orientation();
//        printf("robot %d: %d %d %f\n", id, newx, newy, newangle);
        if(uniqueBotIDs.find(id) != uniqueBotIDs.end())
          continue;
        uniqueBotIDs.insert(id);
       // printf("newxy : %d %d\n", newx, newy);
        
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif
        double           delTime = timeCapture - homeLastUpdateTime[id];
		double timeMs = delTime ; //(nowTime - bsQ.front().second)*1000.0;
		if(timeMs <= 0){
			timeMs = 0.001;
		}
        static float lastnx = 0;
		float  lastPoseX        = homePose[id].x;
		float  lastPoseY         = homePose[id].y;
		float  lastAngle         = homeAngle[id];
		float lastVelocityx      = homeVelocity[id].x;
        float lastVelocityy      = homeVelocity[id].y;
        float lastAngularV       = homeOmega[id];
		homePose[id].x = newx;
		homePose[id].y = newy;

    std::cout<<"HOMEPOSE KALMAN 123 "<<homePose[id].x<<" "<<homePose[id].y<<std::endl;
		homeAngle[id] = newangle;
		homeVelocity[id].x = (newx - lastPoseX)/delTime;
		homeVelocity[id].y = (newy - lastPoseY)/delTime;
        homeAcc[id].x            = (homeVelocity[id].x - lastVelocityx) / delTime;
		homeAcc[id].y           = (homeVelocity[id].y - lastVelocityy) / delTime;
        homeOmega[id]            = (homeAngle[id] - lastAngle) / delTime;
        homeAngularAcc[id]       = (homeOmega[id] - lastAngularV) / delTime;
		
        if(homeAngle[id] > PI)  
			homeAngle[id] -=2*PI;
        else if(homeAngle[id] <=-PI)
			homeAngle[id] += 2*PI;
        homeLastUpdateTime[id]   = timeCapture;
        
		//Adding vl,vr calculation from motion-simulation
		homeVlVr[id] = calcBotVelocity((newx - lastPoseX)/fieldXConvert , (newy - lastPoseY)/fieldXConvert, newangle, lastAngle, timeMs);
        checkValidX(homePose[id].x, homeVelocity[id].x, newx);
        checkValidY(homePose[id].y, homeVelocity[id].y, newy);
     //   checkValidA(homeAngle[id], homeOmega[id], newangle);
      }
      // Yellow  Away robot info
      uniqueBotIDs.clear();
      for (int i = 0; i < yellowNum; ++i)
      {
        
        fira_message::Robot robot = frame.robots_yellow(i);
		    int newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;
        float newangle = robot.orientation();
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif
        //int                id    = HAL::YellowMarkerMap[robot.robot_id()];				
				/* Arpit: Modifying code here to assuming that NO botid of opp team is known. Plz be careful!
				 * Randomly assigning IDs to each bot. if less no. of bots know, then 0...i ids will be populated, 
				 * i+1...4 ids will not be used. 
				 */
				int id_ = getClosestBotID(newx, newy, newangle, uniqueBotIDs);
				if(id_ == -1) //means all bots already populated
					continue;
				int id = HAL::YellowMarkerMap[id_];
				uniqueBotIDs.insert(id);
        //printf("ID = %d, loc = %d, %d\n", id, newx, newy);
        double           delTime = timeCapture - awayLastUpdateTime[id];
		float  lastPoseX         = awayPose[id].x;
		float  lastPoseY         = awayPose[id].y;
		float  lastAngle         = awayAngle[id];
		float lastVelocityx      = awayVelocity[id].x;
        float lastVelocityy      = awayVelocity[id].y;
        float lastAngularV       = awayOmega[id];
		awayPose[id].x = newx;
		awayPose[id].y = newy;
		awayAngle[id] = newangle;
		awayVelocity[id].x = (newx - lastPoseX)/delTime;
		awayVelocity[id].y = (newy - lastPoseY)/delTime;
        awayAcc[id].x            = (awayVelocity[id].x - lastVelocityx) / delTime;
		awayAcc[id].y           = (awayVelocity[id].y - lastVelocityy) / delTime;
        awayOmega[id]            = (awayAngle[id] - lastAngle) / delTime;
        awayAngularAcc[id]       = (awayOmega[id] - lastAngularV) / delTime;
		
        if(awayAngle[id] > PI)       
			awayAngle[id] -= 2*PI;
        else if(awayAngle[id] <=-PI) 
			awayAngle[id] += 2*PI;
        awayLastUpdateTime[id]   = timeCapture;
		
		//Adding vl,vr calculation from motion-simulation
		
        checkValidX(awayPose[id].x, awayVelocity[id].x, newx);
        checkValidY(awayPose[id].y, awayVelocity[id].y, newy);
        checkValidA(awayAngle[id], awayOmega[id], newangle);
        //printf("Awaybots %d: %f %f %f %f %lf\n", id, awayPose[id].x, awayPose[id].y, predictedPoseX, awayPosK[id].y, delTime);
      }
    }
    else
    {
      // Home Yellow robot info
      uniqueBotIDs.clear();
      for (int i = 0; i < yellowNum; ++i)
      {
        fira_message::Robot robot = frame.robots_yellow(i);
        int id    = HAL::YellowMarkerMap[robot.robot_id()];
        if(uniqueBotIDs.find(id) != uniqueBotIDs.end())
          continue;
        uniqueBotIDs.insert(id);
        int  newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;   
        std::cout<<"Robot"<<i<<" "<<newx<<" "<<newy<<std::endl;
		float newangle;

		if(robot.orientation())
			newangle = robot.orientation();
		else		// SSL_GeometryFieldSize fiel;
		// fiel = geometry.field();
			newangle = 0;
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif
        double           delTime = timeCapture - homeLastUpdateTime[id];
	//	double timeMs = delTime ; //(nowTime - bsQ.front().second)*1000.0;
	//	if(timeMs <= 0){
	//		timeMs = 0.001;
	//	}		
		float  lastPoseX         = homePose[id].x;
		float  lastPoseY         = homePose[id].y;
		float  lastAngle         = homeAngle[id];
		float lastVelocityx      = homeVelocity[id].x;
        float lastVelocityy      = homeVelocity[id].y;
        float lastAngularV       = homeOmega[id];
		homePose[id].x = newx;
		homePose[id].y = newy;
		homeAngle[id] = newangle;
		homeVelocity[id].x = (newx - lastPoseX)/delTime;
		homeVelocity[id].y = (newy - lastPoseY)/delTime;
        homeAcc[id].x            = (homeVelocity[id].x - lastVelocityx) / delTime;
		homeAcc[id].y           = (homeVelocity[id].y - lastVelocityy) / delTime;
        homeOmega[id]            = (homeAngle[id] - lastAngle) / delTime;
        homeAngularAcc[id]       = (homeOmega[id] - lastAngularV) / delTime;
        
		if(homeAngle[id] > PI)
			homeAngle[id] -= 2*PI;
        else if(homeAngle[id] <=-PI) 
			homeAngle[id] += 2*PI;
        checkValidX(homePose[id].x, homeVelocity[id].x, newx);
        checkValidY(homePose[id].y, homeVelocity[id].y, newy);
        checkValidA(homeAngle[id], homeOmega[id], newangle);
        homeLastUpdateTime[id]   = timeCapture;
		
		//Adding vl,vr calculation from motion-simulation
		homeVlVr[id] = calcBotVelocity((newx - lastPoseX)/fieldXConvert, (newy - lastPoseY)/fieldXConvert, newangle, lastAngle, delTime);
		//cout << (newx - lastPoseX)/fieldXConvert << " " << (newy - lastPoseY)/fieldXConvert << " " << newangle << " " << lastAngle << " " << delTime << endl;
	    myfileX <<" homeVlVr[id] :: "<<homeVlVr[id].x<<"  "<<homeVlVr[id].y<<endl;
	  }
	  
      // Blue robot info
      uniqueBotIDs.clear();
      for (int i = 0; i < blueNum; ++i)
      {        
        fira_message::Robot robot = frame.robots_blue(i);        
        int newx = robot.x() - CENTER_X;
        int newy = robot.y() - CENTER_Y;
        float newangle = robot.orientation();
        #if GR_SIM_COMM || FIRASSL_COMM
        linearTransform(newx, newy, newangle);
        #endif
        #if FIRASSL_COMM
        botcenterTransform(newx, newy, newangle);
        #endif						
				/* Arpit: Modifying code here to assuming that NO botid of opp team is known. Plz be careful!
				 * Randomly assigning IDs to each bot. if less no. of bots know, then 0...i ids will be populated, 
				 * i+1...4 ids will not be used. 
				 */
		int id_ = getClosestBotID(newx, newy, newangle, uniqueBotIDs);
		if(id_ == -1) //means all bots already populated
				continue;
		int id = HAL::BlueMarkerMap[id_];
		uniqueBotIDs.insert(id);
        //printf("ID = %d, loc = %d, %d\n", id, newx, newy);
				
				
        double           delTime = timeCapture - awayLastUpdateTime[id];
		float  lastPoseX         = awayPose[id].x;
		float  lastPoseY         = awayPose[id].y;
		float  lastAngle         = awayAngle[id];
		float lastVelocityx      = awayVelocity[id].x;
        float lastVelocityy      = awayVelocity[id].y;
        float lastAngularV       = awayOmega[id];
		awayPose[id].x = newx;
		awayPose[id].y = newy;
		awayAngle[id] = newangle;
		awayVelocity[id].x = (newx - lastPoseX)/delTime;
		awayVelocity[id].y = (newy - lastPoseY)/delTime;
        awayAcc[id].x            = (awayVelocity[id].x - lastVelocityx) / delTime;
		awayAcc[id].y           = (awayVelocity[id].y - lastVelocityy) / delTime;
        awayOmega[id]            = (awayAngle[id] - lastAngle) / delTime;
        awayAngularAcc[id]       = (awayOmega[id] - lastAngularV) / delTime;
        if(awayAngle[id] > PI)        
			awayAngle[id] -= 2*PI;
        else if(awayAngle[id] <=-PI)  
			awayAngle[id] += 2*PI;
        checkValidX(awayPose[id].x, awayVelocity[id].x, newx);
        checkValidY(awayPose[id].y, awayVelocity[id].y, newy);
        checkValidA(awayAngle[id], awayOmega[id], newangle);
        
        awayLastUpdateTime[id]   = timeCapture;
        //printf("Awaybots %d: %f %f %f %f %lf\n", id, awayPose[id].x, awayPose[id].y, predictedPoseX, awayPosK[id].y, delTime);
      }
    }

	bsQ.pop();
	bsQ.push(std::make_pair(BeliefState(), nowTime));
   mutex->leave();
  }

  void Kalman::update(BeliefState& state)
  {
    mutex->enter();
    // TODO : Add delTime here and update the state state with respect to delTime
    struct timeval tv2;
    gettimeofday(&tv2, NULL);
    double timeNow      = tv2.tv_sec+tv2.tv_usec/1000000.0;
    state.currFrameNum++;
	
	/* new adds for ourNum and oppNum*/
    if (HomeTeam::COLOR == Simulator::BLUE_TEAM)
    {
      state.ourNum = blueNum;
      state.oppNum = yellowNum;
    }
    else if(HomeTeam::COLOR == Simulator::YELLOW_TEAM)
    {
      state.ourNum = yellowNum;
      state.oppNum = blueNum;
    }
    else
    {
      assert(HomeTeam::COLOR == Simulator::YELLOW_TEAM || HomeTeam::COLOR == Simulator::BLUE_TEAM);
    }
	
    for (int botID = 0; botID < HomeTeam::SIZE; ++botID)
    {
      double delTime = 0;
      state.homePos[botID]   = Vector2D<int>(homePose[botID].x, homePose[botID].y);/*Vector2D<int>(homePose[botID].x+homeVelocity[botID].x*delTime, 
      homePose[botID].y+homeVelocity[botID].y*delTime);*/
      printf("Home Pose Kalman: %d %d %d\n",botID,state.homePos[botID].x,state.homePos[botID].y);
	  state.homeAngle[botID] = homeAngle[botID];// + homeOmega[botID]*delTime;
      state.homeVel[botID]   = homeVelocity[botID];
	  state.homeVlVr[botID] = homeVlVr[botID];
	  // set the sent vl, vr from comm
	  {
		int vl, vr;		
		SkillSet::comm->getSentData(botID, vl, vr);
		state.homeSentVlVr[botID].x = vl;
		state.homeSentVlVr[botID].y = vr;
	  }	  
      state.homeOmega[botID] = homeOmega[botID];
      state.homeAcc[botID]   = homeAcc[botID];
      state.homeAngAcc[botID]= homeAngularAcc[botID];
    }
    //printf("updating %d %d\n", state.homePos[1].x, state.homePos[1].y);
    for (int botID = 0; botID < AwayTeam::SIZE; ++botID)
    {
      double delTime = 0;
      state.awayPos[botID]   = Vector2D<int>(awayPose[botID].x, awayPose[botID].y);
      state.awayAngle[botID] = awayAngle[botID];
      state.awayVel[botID]   = awayVelocity[botID];
      state.awayOmega[botID] = awayOmega[botID];
      state.awayAcc[botID]   = awayAcc[botID];
      state.awayAngAcc[botID]= awayAngularAcc[botID];
    }
      double delTime = 0;
	  
      state.ballPos = Vector2D<int>(ballPose.x + delTime*ballVelocity.x, ballPose.y + delTime*ballVelocity.y);
      state.ballVel = ballVelocity;
      state.ballAcc = ballAcceleration;
	  state.prevBallVel = prevBallVelocity;
	//  cout << ballAcceleration.x << " " << ballAcceleration.y << endl;
    //printf("Omega-> %lf\n",state.homeOmega[2]);

        /*for (int botID = 0; botID < HomeTeam::SIZE; ++botID)
    {
        if((v.readBuffer.botArr[HOME_TEAM][botID].center.x != 0) && (v.readBuffer.botArr[HOME_TEAM][botID].center.y != 0))
        {
      state.homePos[botID]   = Vector2D<int>(v.readBuffer.botArr[HOME_TEAM][botID].center.x,v.readBuffer.botArr[HOME_TEAM][botID].center.y);
      
      //state.homeAngle[botID] = homeAngle[botID];
          state.homeAngle[botID] = v.readBuffer.botArr[HOME_TEAM][botID].orientation;
        }
      //state.homeVel[botID]   = homeVelocity[botID];
      //state.homeOmega[botID] = homeOmega[botID];
                        //state.homeAcc[botID]   = homeAcc[botID];
                        //state.homeAngAcc[botID]= homeAngularAcc[botID];
    }
        for (int botID = 0; botID < AwayTeam::SIZE; ++botID)
    {
      state.awayPos[botID]   = Vector2D<int>(v.readBuffer.botArr[AWAY_TEAM][botID].center.x,v.readBuffer.botArr[AWAY_TEAM][botID].center.y);
      //state.homeAngle[botID] = homeAngle[botID];
      state.awayAngle[botID] = v.readBuffer.botArr[AWAY_TEAM][botID].orientation;
      //state.homeVel[botID]   = homeVelocity[botID];
      //state.homeOmega[botID] = homeOmega[botID];
                        //state.homeAcc[botID]   = homeAcc[botID];
                        //state.homeAngAcc[botID]= homeAngularAcc[botID];
    }
    state.ballPos = Vector2D<int>(v.readBuffer.ball.x,v.readBuffer.ball.y);
    printf("Ball pos %d,%d\n",state.ballPos.x,state.ballPos.y);
    printf("Aqua bot %d,%d\n",state.homePos[1].x,state.homePos[1].x);*/
    
    state.computeBallLocation();
    state.computeBallInDBox();
    //state.computeBallSide();
    state.computeBallInStrips();
    mutex->leave();
  }


#endif



}