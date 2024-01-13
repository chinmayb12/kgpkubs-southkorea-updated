#ifndef KALMAN_H
#define KALMAN_H

#include <cstdio>
#include "visionThread.h"
#include "geometry.hpp"
#include "../common/include/timer.h"
#include <queue>
#include <utility>
#include <set>
#include <fstream>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "sgfilter.h"
#include "common.pb.h"
#include "packet.pb.h"
#include "replacement.pb.h"

using namespace std;
using namespace cv;
// Forward Declarations
namespace Util
{
  class CS;
}

namespace Strategy
{
  class BeliefState;
}

class SSL_DetectionFrame;

namespace Strategy
{
  const float SIGMA_SQ_NOISE_POS   = 1;//1.0f;
  const float SIGMA_SQ_OBSVN_POS   = 0.1;//0.03f;
  const float SIGMA_SQ_NOISE_ANGLE = 0.01;
  const float SIGMA_SQ_OBSVN_ANGLE = 0.01;

#ifdef VISION_COMM
  class Kalman
  {
  private:
    FILE* kalmanlog;
		int getClosestBotID(int x, int y, float angle, std::set<int> &uniqueBotIDs); /* Arpit: returns the closest opp bot to this location. */
	public:

	/* (phi, delta_phi) */
    Kalman();
    
    ~Kalman();
	
	int blueNum ;
    int yellowNum ;
    Vector2D<float> homePose[HomeTeam::SIZE];
    Vector2D<float> awayPose[AwayTeam::SIZE];
    Vector2D<float> homeVelocity[HomeTeam::SIZE];
	Vector2D<float> homeVlVr[HomeTeam::SIZE];
    Vector2D<float> awayVelocity[AwayTeam::SIZE];
    Vector2D<float> homeAcc[HomeTeam::SIZE];
    Vector2D<float> awayAcc[AwayTeam::SIZE];
    Vector2D<float> homePosK[HomeTeam::SIZE];
    Vector2D<float> awayPosK[AwayTeam::SIZE];
    Vector2D<float> homePosSigmaSqK[HomeTeam::SIZE];
    Vector2D<float> awayPosSigmaSqK[AwayTeam::SIZE];

    float           homeAngle[HomeTeam::SIZE];
    float           awayAngle[AwayTeam::SIZE];
    float           homeOmega[HomeTeam::SIZE];
    float           awayAngularAcc[AwayTeam::SIZE];
    float           homeAngularAcc[HomeTeam::SIZE];
    float           awayOmega[AwayTeam::SIZE];
    float           homeAngleK[HomeTeam::SIZE];
    float           awayAngleK[AwayTeam::SIZE];
    float           homeAngleSigmaSqK[HomeTeam::SIZE];
    float           awayAngleSigmaSqK[AwayTeam::SIZE];

    double          homeLastUpdateTime[HomeTeam::SIZE];
    double          awayLastUpdateTime[AwayTeam::SIZE];
    Util::Timer      t;

    Vector2D<float> ballPose;
    Vector2D<float> ballVelocity;
    Vector2D<float> ballAcceleration;
	Vector2D<float> prevBallVelocity;
    Vector2D<float> ballPosK;
    Vector2D<float> ballPosSigmaSqK;
	Vector2D<float> left_corner;
	Vector2D<float> right_corner;
	Vector2D<float> left_center;
	Vector2D<float> right_center;
    double          ballLastUpdateTime;

    Util::CS*        mutex;
    
    void addInfo(SSL_DetectionFrame& detection);
    void update(BeliefState& state);
	  void updateField(SSL_GeometryData& geometry);
	  std::deque<Vector2D<float> > QueueVel, QueuePos;
	  std::deque<Vector2D<float> > ballPosQueue;
	  std::queue<std::pair<BeliefState, double> > bsQ;
	  static const int MAX_BS_Q = 2;
	
	ofstream myfileX, myfileY; 
	Vector2D<float> calcBotVelocity(double delX, double delY, double Theta1, double Theta2, float timeMs);
	vector<float> ballPosX, ballPosY, ballPosTimeX, ballPosTimeY;
   };

#else

class Kalman{
  private:
    FILE* kalmanlog;
		int getClosestBotID(int x, int y, float angle, std::set<int> &uniqueBotIDs); /* Arpit: returns the closest opp bot to this location. */
	public:
	/* (phi, delta_phi) */
    Kalman();
    
    ~Kalman();
	
	int blueNum ;
    int yellowNum ;
    Vector2D<float> homePose[HomeTeam::SIZE];
    Vector2D<float> awayPose[AwayTeam::SIZE];
    Vector2D<float> homeVelocity[HomeTeam::SIZE];
	Vector2D<float> homeVlVr[HomeTeam::SIZE];
    Vector2D<float> awayVelocity[AwayTeam::SIZE];
    Vector2D<float> homeAcc[HomeTeam::SIZE];
    Vector2D<float> awayAcc[AwayTeam::SIZE];
    Vector2D<float> homePosK[HomeTeam::SIZE];
    Vector2D<float> awayPosK[AwayTeam::SIZE];
    Vector2D<float> homePosSigmaSqK[HomeTeam::SIZE];
    Vector2D<float> awayPosSigmaSqK[AwayTeam::SIZE];

    float           homeAngle[HomeTeam::SIZE];
    float           awayAngle[AwayTeam::SIZE];
    float           homeOmega[HomeTeam::SIZE];
    float           awayAngularAcc[AwayTeam::SIZE];
    float           homeAngularAcc[HomeTeam::SIZE];
    float           awayOmega[AwayTeam::SIZE];
    float           homeAngleK[HomeTeam::SIZE];
    float           awayAngleK[AwayTeam::SIZE];
    float           homeAngleSigmaSqK[HomeTeam::SIZE];
    float           awayAngleSigmaSqK[AwayTeam::SIZE];

    double          homeLastUpdateTime[HomeTeam::SIZE];
    double          awayLastUpdateTime[AwayTeam::SIZE];
    Util::Timer      t;

    Vector2D<float> ballPose;
    Vector2D<float> ballVelocity;
    Vector2D<float> ballAcceleration;
	Vector2D<float> prevBallVelocity;
    Vector2D<float> ballPosK;
    Vector2D<float> ballPosSigmaSqK;
	Vector2D<float> left_corner;
	Vector2D<float> right_corner;
	Vector2D<float> left_center;
	Vector2D<float> right_center;
    double          ballLastUpdateTime;

    Util::CS*        mutex;
    
    void addInfo(fira_message::Frame& frame);
    void update(BeliefState& state);
	  void updateField(fira_message::Field& geometry);
	  std::deque<Vector2D<float> > QueueVel, QueuePos;
	  std::deque<Vector2D<float> > ballPosQueue;
	  std::queue<std::pair<BeliefState, double> > bsQ;
	  static const int MAX_BS_Q = 2;
	
	ofstream myfileX, myfileY; 
	Vector2D<float> calcBotVelocity(double delX, double delY, double Theta1, double Theta2, float timeMs);
	vector<float> ballPosX, ballPosY, ballPosTimeX, ballPosTimeY;
};

#endif

  
}

#endif // KALMAN_H