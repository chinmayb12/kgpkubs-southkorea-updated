// It is through the skill layer that the Strategy layer talks to the Simulator/Actual robots
#pragma once
#ifndef SKILLS_H
#define SKILLS_H
#include <dlib/svm.h>
#if FIRASSL_COMM||FIRA_COMM
# include "fira_comm.h"
#else
# error Macro for Comm class not defined
#endif
#include <vector>
#include "pathPlanners.h"
#include "sslDebug_Data.pb.h"
#include "pose.h"
#include "controller-wrapper.hpp"
#include "trajectory.hpp"
#include <deque>
#include <fstream>

// Forward Declarations
namespace Strategy
{
  class MergeSCurve;
  class LocalAvoidance;
  class BeliefState;
}
namespace HAL {
  class Comm;
}
template <class T> class Vector2D;
namespace Strategy
{
  /* Procedure for adding a skill successfully to the SkillSet is:
   * 1. Enumarate it in enum SkillSet::SkillID just before MAX_SKILLS
   * 2. Identify its input parameters and define a struct for it in union SParam
   * 3. Declare the function prototype for the skill. Its parameter must only be SParam& param
   * 4. Map the Skill ID to the corresponding function in the constructor of SkillSet
   * 5. Define the function that implements the skill declared in step 3.
   * NOTE: Variable names of enums and corresponding skill function names must be the same,
   *       except that the enum name starts with a capital letter. Name of the corresponding
   *       struct should be the same as that of enum with a trailing 'P' character
   */
   
  // Union of parameters for each skill enumerated in SkillID
    union SParam
    {
      // Parameters for the skill Spin
      struct type1
      {
        float radPerSec;
      } SpinP;

      // Parameters for the skill Stop
      struct type2
      {} StopP, GoalKeepingP;

      // Parameters for the skill Kick
      struct type3
      {
        float x;
        float y;
        float finalSlope;
        float finalVelocity;
		bool initTraj;
      } SplineGoToPointP, DWGoToPointP;

      // Parameters for the skill Velocity
      struct type4
      {
        float vl;
        float vr;
      } VelocityP;

      //Parameters for skill goToPoint and dribble to point
      struct type5
      {
        float x;
        float y;
        float finalslope;
        float radius;
      } DefendPointP;
      struct type6
      {
        float finalslope;
      } TurnToAngleP;
      struct type7
      {
        bool align;
      } GoToBallP , GoBehindBallP;
      struct type8
      {
        float x;
        float y;
        float finalslope;
        bool align;
        float finalVelocity;
		//Parameters for learning?? not needed
        float bot_point_dist_penalty;
        float angle_diff_penalty;
        bool increaseSpeed;
        bool isBallObstacle ;
      } GoToPointP;
	  struct type9
      {
        float vl;
        float vr;
		float ballVelX;
		float ballVelY;
		bool velGiven;
		bool initTraj;
		bool changeSpline;
      }SplineInterceptBallP;

    };
  class SkillSet
  {
	private:
		ControllerWrapper *algoController;
		Trajectory* traj;
		int counter, interceptCounter, sCount, splineTrack, sTrack;
		ofstream myfileX, myfileY; 
	public:
		enum SkillID
		{
			Spin,
			Stop,
			Velocity,
			GoToBall,
			GoToPoint,
			TurnToAngle,
			DefendPoint,
			ChargeBall,
			SplineGoToPoint,
			DWGoToPoint,
			SplineInterceptBall,
			GoBehindBall ,
			MAX_SKILLS
		};
	
  protected:
    typedef void (SkillSet::*skillFncPtr)(const SParam&);
    const BeliefState* state;
    const int          botID;
    skillFncPtr        skillList[MAX_SKILLS];
#ifdef LOCAL_AVOID
    LocalAvoidance*    pathPlanner;
    MergeSCurve*       pathPlannerSec;
#else
    MergeSCurve*       pathPlanner;
#endif

    bool pointyInField(Vector2D<int> fianl);
    void _goToPoint(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance,bool increaseSpeed=0);
	int prevVel;
    /***************************By Prasann***********/
    /* Arpit: params removed from here, made static in function itself. I wanted to make function static so that
     * it can be called from sendCommand or tester itself. */
    /***************************By Prasann***********/
	public:
	
    static HAL::Comm*    comm;
    //------- List of robot skills -------//
    void spin(const SParam& param);
    void stop(const SParam& param);
    void velocity(const SParam& param);
    void goToBall(const SParam& param);
    void goToPoint(const SParam& param);
    void turnToAngle(const SParam& param);
    void defendPoint(const SParam& param);
	void chargeBall(const SParam& param);
	void goBehindBall(const SParam &param) ;
    void splineGoToPoint(const SParam& param);
	void _splineGoToPointTrack(int botid, Pose start, Pose end, float finalvel, float vls, float vrs);
	void _splineGoToPointInitTraj(int botid, Pose start, Pose end, float finalvel, float vls, float vrs, int flag);
	bool _isFrontDirected(Pose botPos, Pose endPos, float vls, float vrs);
	void dwGoToPoint(const SParam& param);
	void _dwGoToPointTrack(int botid, Pose start, Pose end, float finalvel);
	void _dwGoToPointInitTraj(int botid, Pose start, Pose end, float finalvel);
	void splineInterceptBall(const SParam& param);
	void _splineInterceptBallInitTraj(int botid, Pose start, Vector2D<float> ballPos, Vector2D<float> ballVel, Vector2D<float> botVel, float final_vl, float final_vr, int initTraj);
	void _splineInterceptBallTrack(int botid, Pose start, Vector2D<float> ballPos, Vector2D<float> ballVel, Vector2D<float> botVel, float final_vl, float final_vr);
    void _splineGoToPointCheckTraj(int botid, Pose start, Pose end, float finalvel, int initTraj);
	void dontRunSpline();
	// Parameter for skills to be trained
    static bool loadParamsFromFile;
    static bool  skillParamsLoaded;
    static float GoToPoint_profileFactorMult;
    static float GoToPoint_omegaFactor;
    static float Bot_Point_dis_penalty_l1;
    static float Bot_Point_dis_penalty_l2;
    static float angle_penalty_l1;
    static float angle_penalty_l2;
    static int dribble_ball_threshold;
    static int bot_ball_threshold;
    static std::string skillsCollection;
    static std::string goToPointCollection;
    /* Error Function by
     * Soumyadeep & Ankit
     */
    static void errorLog(int botid,float vl,float vr, const BeliefState *state);
	// old predictions side-by-side with the actual position of the bot.
    std::deque<Pose> predictedPoseQ;
    
    SkillSet(const BeliefState* state, int botID);
    ~SkillSet();
    // Executing the skill function corresponding to the given ID and param function parameters
    inline void executeSkill(SkillID ID, const SParam& param)
    {
      std::cout<<"ID "<<ID<<std::endl;
      (*this.*skillList[ID])(param);
    } // executeSkill
    
    //QtDebugger Compatibility Functions
	static void addCircle(int x, int y, unsigned int radius, unsigned int color)
	{
		comm->addCircle(x, y, radius, color);
	}
	static void addLine(int x1, int y1, int x2, int y2, unsigned int color )
	{
		comm->addLine(x1, x2, y1, y1, color);
	}
    static std::list<Debug_Circle> getCircles()
    {
      return comm->getCircles();
    }
    static std::list<Debug_Line> getLines()
    {
      return comm->getLines();
    }
    static void clearDebugData()
    {
#ifdef FIRASSL_COMM
      comm->clearDebugData();
#endif
    }
  }; // class SkillSet
} // namespace Strategy
#endif // SKILLS_H
