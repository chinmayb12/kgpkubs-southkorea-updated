#include <config.h>

//#define SGN(x) (((x)>0)?1:(((x)<0)?(-1):0))
//extern bool FIELD_IS_INVERTED;


const int GOAL_DEPTH                   = SELECT(300, 300);
const float MOVING_BALL_VELOCITY       = SELECT(40, 30);
const float MIN_DIST_FROM_TARGET       = SELECT(30.0, 25.0);
const int CENTER_X                     = SELECT(0, 0);
const int CENTER_Y                     = SELECT(0, 0);
const int HALF_FIELD_MAXX              = SELECT(2800, 2975); //actual 225 (rugged surface at end) // WAS 2800
const int HALF_FIELD_MAXY              = SELECT(1900, 2050);

const int OUR_GOAL_MAXY                = SELECT(600,600);
const int OUR_GOAL_MINY                = SELECT(-600, -600);
const int OPP_GOAL_MAXY                = SELECT(600, 600);
const int OPP_GOAL_MINY                = SELECT(-600, -600);
const int OUR_GOAL_WIDTH               = OUR_GOAL_MAXY - OUR_GOAL_MINY;
const int OPP_GOAL_WIDTH               = OPP_GOAL_MAXY - OPP_GOAL_MINY;
const int CENTER_CIRCLE_DIAMETER       = SELECT(1000,1000);
const int DBOX_WIDTH                   = SELECT(600,1200);       //Along X direction
const int DBOX_HEIGHT                  = SELECT(600,855);       //Along Y direction(half height in each y direction)
const int DBOX_DEPTH                   = SELECT(10, 10);
const int BALL_AT_CORNER_THRESH        = SELECT(20,20); 
/* Bot Parameteres configuration */
const float ROTATION_FACTOR            = SELECT(0.05, 0.15);                //

const float RFACTOR                    = SELECT(3,   0.3);
const float RFACTOR_SMALL              = SELECT(0.6, 0.15);

const int CLEARANCE_PATH_PLANNER       = SELECT(500, 400);              //mm
const int MID_FIELD_THRESH             = SELECT(10, 150);                       // mm
const float BOT_RADIUS                 = SELECT(70, 150);                       // mm
const float BALL_RADIUS                = SELECT(40, 40);                       // mm
const float SAFE_RADIUS                = (BOT_RADIUS * 2);
const float COLLISION_DIST             = (BOT_RADIUS * 7);
const int DRIBBLER_BALL_THRESH         = SELECT(110, 500);            // mm
const int FREEKICK_RADIUS							 = SELECT(25,650);
const int FREEBALL_RADIUS							 = SELECT(30,700);
const int KICKOFF_RADIUS	             = SELECT(18,200);


const int BOT_BALL_THRESH              = SELECT(105, 150);                  // mm
const int BOT_BALL_THRESH_FOR_PR       = SELECT(105, 200);                  // mm
const int BOT_POINT_THRESH             = SELECT(105, 147);                     // mm

const int STRIP_WIDTH_X                = ForwardX(BOT_RADIUS*1.5);
const int STRIP_WIDTH_Y                = BOT_RADIUS*1.5;
const int MAX_FIELD_DIST               = SELECT(1000, 3500);                // mm
const float MAX_WHEEL_SPEED            = SELECT(2000, 100);                 //mm/s
const float MAX_BOT_LINEAR_ACC         = SELECT(1000, 100);                // mm/s/s
const float MAX_BOT_LINEAR_VEL_CHANGE  = SELECT(10, 4);
const float MAX_BOT_SPEED              = SELECT(1800, 120.0);                 // mm
const float MIN_BOT_SPEED              = SELECT(5, 10);                     // mm/s
const float MAX_BOT_OMEGA              = SELECT(7, 100);                     // rad/s//2
const float MIN_BOT_OMEGA              = SELECT(0.15, 0.15);                     // rad/s
const float MAX_BACK_DRIBBLE_V_Y       = SELECT(500, 500);                  // mm/s
const float MAX_FRONT_DRIBBLE_V_Y      = SELECT(1200, 1200);                // mm/s
const float MAX_DRIBBLE_V_X            = SELECT(200, 100);                 // mm/s
const float MAX_DRIBBLE_R              = SELECT(3, 3);                      // rad
const float DRIBBLER_BALL_ANGLE_RANGE  = SELECT(0.2f, 0.10f);                // rad
const float SATISFIABLE_THETA          = SELECT(0.08f, 0.1f);                // rad
const float SATISFIABLE_THETA_SHARP    = SELECT(0.01f, 0.01f);              // rad
const float MAX_BALL_STEAL_DIST        = SELECT(800, 800);
/// SSL param. not needed. 
const int MAX_KICK_SPEED               = SELECT(0,0);
/* If the velocity of a bot is below this value, then the bot has effectively zero velocity */
const float ZERO_VELOCITY_THRESHOLD    = SELECT(10, 10);
const float ZERO_VELOCITY_THRESHOLD_SQ = (ZERO_VELOCITY_THRESHOLD * ZERO_VELOCITY_THRESHOLD);
const float LOW_BALL_VELOCITY_THRES    = SELECT(50, 50);
const float LOW_BALL_VELOCITY_THRES_SQ = (LOW_BALL_VELOCITY_THRES*LOW_BALL_VELOCITY_THRES);
// Parameters useful for camera's data transformation.
const int OUR_GOAL_Y = 500;
const int OPP_GOAL_Y = 0;
const float NETWORK_DELAY = SELECT(0.05, 0.05);  // Network Delay in miliseconds
//Distance Hysteresis factor for switching of roles
const int HYSTERESIS = SELECT(20000, 300);

const short STRATEGY_GUI_MULTICAST_PORT = 10001 ;
const std::string STRATEGY_GUI_MULTICAST_ADDR = "224.5.23.1" ;

// Parameters useful for camera's data transformation.
const double d          = 6.8; //distance between wheels in cm
const double ticksToCmS = 1.107; //still only approximate... v = v_ticks * ticksToCmS 1.54
const double fieldXConvert = 23.79; // now im always using xconvert as standard conversion from strategy -> cm and vice versa.
const double fieldYConvert = fieldXConvert;
// NOTE(arpit): Uncertainties should be non-zero when simulating. Currently 0 since bot data is fetched from vision.
const double xUncertainty = 0;//0.5; // Uncertainty is in %age of max value. eg. 1% where fabs(x) <= 1000 means fabs(error) <= 10
const double yUncertainty = 0;//0.5;
const double thetaUncertainty = 0;//3;
// NOTE(arpit): numPacketDelay and update() specified here is only used in simulation.
const int numPacketDelay = 0; // num of packets to delay in update

const double vwSlope    = -5.92; //making vw as function of r
const double vwIntercept = 700;
const double vwmax      = 200; // cm/s^2
const double vsat       = ticksToCmS*110.; // cm/s   
const double atmax      = 100*ticksToCmS; // cm/s^2, need to measure this, need to take inertia into account
const double awmax      = 1000; // 1/s^2, no idea how to measure this, need to take inertia into account
const double ballacc    = 0.8;
