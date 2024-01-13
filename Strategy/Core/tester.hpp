#include "mainheaders.hpp"
#include "thread.h"
#include "strategygui_thread.hpp"
#include "command-packet.pb.h"
#include "tExec.h"

#include <fstream>

using namespace std;
using namespace Strategy;

class Tester : public Util::Thread
{
public:
  bool &running;
  BeliefState &state;
  Tester(bool &running_, BeliefState &state_):
  running(running_),state(state_) {}
  void run()
  {
	
    Util::CS loggerCS;
    Util::Logger logger("timelog.log", Util::Logger::Write, loggerCS);
    Kalman      kFilter ;
    VisionThread vThread(&kFilter);
    vThread.start();
//	kFilter.Kalman();
    #ifdef STR_GUI
    Util::CS strCS;
	
    StrategyPacket strPktSh;
    StrategyGUIThread strGUIThread = StrategyGUIThread::getInstance(&strPktSh, &strCS);
    strGUIThread.start();
	TExec tExec(&state);
	
	
	int prev_tactic_id = -1;
	int prev_bot_id = -1;
	int prev_play_id = 0;
	PExec       pExec(&state);
	pExec.selectfromGUI(4); // calling stop as soon as the program is started
    #endif
	Tactic::Param paramList[Tactic::MAX_TACTICS];
	
    //tGoalKeeping
    TGoalKeepingOurSide tGoalOur0(&state, 0);
    TGoalKeepingOurSide tGoalOur1(&state, 1);
	TGoalKeepingOurSide tGoalOur2(&state, 2);
	TGoalKeepingOurSide tGoalOur3(&state, 3);
	TGoalKeepingOurSide tGoalOur4(&state, 4);
    Tactic::Param paramGoal;
	
	TGoalie2 tGoalie2(&state, 0);
     
	TShoot tShoot4(&state , 4) ;
	Tactic::Param paramShoot;
	
	TDefender2 defender2_4(&state, 4);
	TDWDefender2015 dwDefend20151(&state, 1);
	Tactic::Param paramDefend2;
	Tactic::Param paramDWDefend2015;
	
    //tCharge
    TCharge tCharge2(&state, 2);
    TCharge tCharge1(&state, 1);
    Tactic::Param pCharge;
    
	
	TReceiveBall2015 tReceive2015_3(&state, 3);
    TReceiveBall2015 tReceive2015_0(&state, 0);
	TReceiveBall2015 tReceive2015_1(&state, 1);
	TReceiveBall2015 tReceive2015_2(&state, 2);
	TReceiveBall2015 tReceive2015_4(&state, 4);
    Tactic::Param pReceive2015; 
	
	TReceiveSpline tReceiveSpline3(&state, 3);
    TReceiveSpline tReceiveSpline0(&state, 0);
	TReceiveSpline tReceiveSpline1(&state, 1);
	TReceiveSpline tReceiveSpline2(&state, 2);
	TReceiveSpline tReceiveSpline4(&state, 4);
    Tactic::Param pReceiveSpline;
	
    //tCover Bot
    TCoverGoal tcover0(&state, 0);
    TCoverGoal tcover1(&state,1);
	TCoverGoal tcover3(&state,3);
    Tactic::Param paramcover;
    paramcover.CoverGoalP.distFromGoal = -2*DBOX_WIDTH;
    paramList[Tactic::CoverGoal].CoverGoalP.distFromGoal = -2*DBOX_WIDTH;
	
	//CoverGoal2015
	TCoverGoal2015 tcover20150(&state,0);
	TCoverGoal2015 tcover20151(&state,1);
	TCoverGoal2015 tcover20152(&state,2);
	TCoverGoal2015 tcover20153(&state,3);
	TCoverGoal2015 tcover20154(&state,4);
	Tactic::Param paramcover2015;
	//CoverGoalPair
	TCoverGoalPairLeft tcoverleft1(&state,1);
	Tactic::Param paramcoverleft;
	TCoverGoalPairRight tcoverright0(&state,0);
	Tactic::Param paramcoverright;
	
	//SupportAttack
	TSupportAttack2015 tsupportattack20150(&state,0);
	Tactic::Param paramsupportattack2015;
	
    //tStop Bot
    TStop tS0(&state, 0);
    TStop tS1(&state, 1);
    TStop tS2(&state, 2);
    Tactic::Param paramStop;
    
    //tposition
    TPosition tPos1(&state, 1);
    TPosition tPos2(&state,2);
    Tactic::Param pPos;
    paramList[Tactic::Position].PositionP.align = false;
    paramList[Tactic::Position].PositionP.x= 0;
    paramList[Tactic::Position].PositionP.y= 0;
    paramList[Tactic::Position].PositionP.finalSlope=PI/2;
    
    //tDefendLine
    TDefendLine tDefendLine1(&state,1);
    TDefendLine tDefendLine2(&state,2);

    Tactic::Param pDefendL1;
    paramList[Tactic::DefendLine].DefendLineP.x1 = 0;//BOT_RADIUS/2;
    paramList[Tactic::DefendLine].DefendLineP.y1 = -HALF_FIELD_MAXY/2;//-BOT_RADIUS/2;;
    paramList[Tactic::DefendLine].DefendLineP.x2 = 0;
    paramList[Tactic::DefendLine].DefendLineP.y2 = HALF_FIELD_MAXY/2;//-HALF_FIELD_MAXY;
	
	
    Tactic::Param pDefendL2;
    pDefendL2.DefendLineP.x1 = BOT_RADIUS/2;
    pDefendL2.DefendLineP.x2 = HALF_FIELD_MAXX/2;
    pDefendL2.DefendLineP.y1 = BOT_RADIUS/2;
    pDefendL2.DefendLineP.y2 = HALF_FIELD_MAXY;
    
    //tVelocity
    TVelocity tVelocity0(&state,0);
	TVelocity tVelocity1(&state,1);
	TVelocity tVelocity3(&state,3);
	TVelocity tVelocity4(&state,4);
	TVelocity tVelocity2(&state,2);
    
	Tactic::Param pVelocity;
	pVelocity.VelocityP.vl = 0;
	pVelocity.VelocityP.vr = 0;
	
	Tactic::Param pVelocity_1;
	pVelocity_1.VelocityP.vl = -140;
	pVelocity_1.VelocityP.vr = -140;
	
	Tactic::Param pVelocity_2;
	pVelocity_2.VelocityP.vl = 60;
	pVelocity_2.VelocityP.vr = 60;
	Tactic::Param pVelocity_3;
	pVelocity_3.VelocityP.vl = 60;
	pVelocity_3.VelocityP.vr = 60;
	Tactic::Param pVelocity_4;
	pVelocity_4.VelocityP.vl = 80;
	pVelocity_4.VelocityP.vr = 80;
	
    paramList[Tactic::Velocity].VelocityP.vl = 60;
    paramList[Tactic::Velocity].VelocityP.vr = 60;
  
    //tDefend Point
    TDefendPoint tDefendPoint1(&state,1);
    Tactic::Param pDefendPoint;
    paramList[Tactic::DefendPoint].DefendPointP.x = -HALF_FIELD_MAXX/2;
    paramList[Tactic::DefendPoint].DefendPointP.y = 0;
    paramList[Tactic::DefendPoint].DefendPointP.radius = HALF_FIELD_MAXX/4;
    
    //tAttack
    TAttack tAttack1(&state, 1);
    TAttack tAttack0(&state, 0);
    TAttack tAttack2(&state, 2);
    TAttack tAttack3(&state, 3);
    TAttack tAttack4(&state, 4);
	 
	TAttackSpline tAttackSpline0(&state , 0);
	TAttackSpline tAttackSpline4(&state , 4);
	TAttackSpline tAttackSpline3(&state , 3);
	TAttackSpline tAttackSpline1(&state , 1);
	TAttackSpline tAttackSpline2(&state , 2);
	
	TSupportAttack2015 tSupportAttack0(&state , 0) ;
    TSupportAttack2015 tSupportAttack1(&state , 1) ;
	
	TAttack2015 tAttack20150(&state , 0) ;
	TAttack2015 tAttack20151(&state , 1) ;
    TAttack2015 tAttack20152(&state , 2) ;
	TAttack2015 tAttack20153(&state , 3) ;
	TAttack2015 tAttack20154(&state , 4) ;
	
    TKickoff tKickoff(&state , 2);
	TPass tPass(&state , 0);
	
	Tactic::Param pAttack;
    paramList[Tactic::Attack].AttackP.rotateOnError = false;
    paramList[Tactic::Attack2015].AttackP.rotateOnError = false;
    // TestgotoPoint
    Strategy::Testgotopoint ttest1(&state,1);
	
	//params1 for SplineGoToPoint
	Strategy::SParam params1;
	params1.SplineGoToPointP.finalVelocity = 0;
	params1.SplineGoToPointP.x = HALF_FIELD_MAXX - GOAL_DEPTH - 6*BOT_RADIUS;
	params1.SplineGoToPointP.y = 0;
	params1.SplineGoToPointP.finalSlope = 0 ;
	params1.SplineGoToPointP.initTraj = 1;
	SkillSet sppoint(&state, 0); 
	
	//params4 for spline interception with ball
	Strategy::SParam params4;
	params4.SplineInterceptBallP.vl = 120;
	params4.SplineInterceptBallP.vr = 120;
	params4.SplineInterceptBallP.initTraj = 1;
	params4.SplineInterceptBallP.changeSpline = true;
	SkillSet sball(&state, 2); 
	
	// params2 for dwgo to point
	Strategy::SParam params2;
	SkillSet dwpoint(&state, 0);
	SkillSet dwpoint_old(&state, 2);
	params2.DWGoToPointP.x = HALF_FIELD_MAXX - GOAL_DEPTH - 6*BOT_RADIUS ;//ForwardX(HALF_FIELD_MAXX);
	params2.DWGoToPointP.y = OUR_GOAL_MAXY;
	params2.DWGoToPointP.finalSlope = 0;
	
	// params3 for mergeS curve
	Strategy::SParam params3 ;
	params3.GoToPointP.x = HALF_FIELD_MAXX - GOAL_DEPTH - 4*BOT_RADIUS;
	params3.GoToPointP.y = OUR_GOAL_MAXY;//HALF_FIELD_MAXY/2;
	params3.GoToPointP.finalslope = 0;
	
	Strategy::SParam params2_old ;
	params2_old.GoToPointP.x = HALF_FIELD_MAXX - GOAL_DEPTH - 6*BOT_RADIUS ;
	params2_old.GoToPointP.y = OUR_GOAL_MINY ;//HALF_FIELD_MAXY/2;
	params2_old.GoToPointP.finalslope = 0;

	Strategy::SParam params3_old ;
	params3_old.DWGoToPointP.x = HALF_FIELD_MAXX - GOAL_DEPTH - 4*BOT_RADIUS;
	params3_old.DWGoToPointP.y = OUR_GOAL_MINY;//HALF_FIELD_MAXY/2;
	params3_old.DWGoToPointP.finalSlope = 0;

	SkillSet simplegoto(&state, 4);
	SkillSet simplegoto_old(&state,1);
    Tactic::Param ptestpoint;
 //   TestbotRace t
	//   test2(&state,2);
  //  Tactic::Param ptestrace;
 //   ptestrace.TestbotRaceP.vl = 100;
  //  ptestrace.TestbotRaceP.vr = 100;
   // FILE *f = fopen("/home/robo/ballPos.txt", "w");  
#ifdef BOTLOG
    FILE *f = fopen("/home/robo/botplot/compare_dataset/botlog.txt", "w");    
    fclose(f);
    f = fopen("/home/robo/botplot/compare_dataset/response.txt", "w");
    fclose(f);
#endif
    bool isRunning = true; 
    //    Util::Timer timer;
	int loopcount = 0;
	unsigned long long int t1=0,t2=0;
	usleep(1000);
			
//	ofstream myfile;
//	myfile.open ("ballPosLog.txt");
	std::cout << "here" << std::endl;
	FILE *f1 = fopen("/tmp/bot_vel_data.txt", "w");
	int aid=4;int rid=4;
	int PtimeID=0;
    while(running)
    {
    //      timer.start();
      state.update();
      kFilter.update(state);
	  {
		int vl, vr;
		SkillSet::comm->getSentData(0, vl, vr);
		fprintf(f1, "sent velocity = (%d, %d), vision velocity = (%f, %f), vision pos = (%f, %f)\n", vl, vr, 
			kFilter.homeVlVr[0].x, kFilter.homeVlVr[0].y, kFilter.homePose[0].x, kFilter.homePose[0].y);
	  }
	  
      if(1)
      {
		//  myfile << state.ballPos.x << " -ballPos- " << state.ballPos.y << endl;
	
		#ifdef STR_GUI
		{
			int test_tactic = 0;
			if(test_tactic){
				strCS.enter();
				//if(strPktSh.which()==1)printf("1\n\n");
				if(strPktSh.tactic().tid() != prev_tactic_id){
					prev_tactic_id = strPktSh.tactic().tid();
					printf("\n\n\n\n\n\n\n");
					printf("****************  HELLO  ******************");
					printf("\n\n\n\n\n\n\n");
				}
				else{
					//printf("****************  BYE BYE  ******************");
				}
		  
				if(strPktSh.tactic().botid() != prev_bot_id){
					prev_bot_id = strPktSh.tactic().botid();
				}
			  
				strCS.leave();
				tExec.execute(&state,(Tactic::ID)(prev_tactic_id-1),paramList[prev_tactic_id -1],prev_bot_id);
				cout << "\n\n From tester: "<< prev_tactic_id-1 << "\t\t" << prev_bot_id;
			}		
			else{
				// Critical Section protected by refBoxCS
				strCS.enter();
				// Updating from Referee Box and the Game State
				//  printf("refBoxCmdSh.cmdCounter = %d\n",refBoxCmdSh.cmdCounter);
				if (strPktSh.play() != prev_play_id)
				{
					printf("GOT COMMAND FROM STRATEGY GUI!!!!\n");
					prev_play_id = strPktSh.play();
					Util::Logger::toStdOut("Command From Refee.. Reselecting play..\n");
					pExec.selectfromGUI(prev_play_id-1);
					
				}
				else if (pExec.playTerminated())
				{
				  Util::Logger::toStdOut("*************Play terminated.Select new play\n*********************");
				  pExec.evaluatePlay();
				  pExec.selectPlay(NULL);
				}
				std::cout<<"Before in test"<<std::endl;
				pExec.executePlay();
				std::cout<<"After in test"<<std::endl;
				strCS.leave();
			}
		}
		#endif
	    
	// tVelocity4.execute(pVelocity_1); 
	 //  simplegoto_old.executeSkill(SkillSet::GoToPoint, params2_old);
	
	// dwpoint.executeSkill(SkillSet::DWGoToPoint,params2) ;
	//tShoot4.execute(paramShoot) ;
		  
	//tGoalOur0.execute(paramGoal);
	// tcover20150.execute(paramcover2015);
     //tcoverleft1.execute(paramcoverleft);
    // tcoverright0.execute(paramcoverright);
    
	// dwDefend20151.execute(paramDWDefend2015);
   
	//tcover20151.execute(paramcover2015);
	 //tAttack20151.execute(pAttack) ;
	//tAttackSpline1.execute(pAttack) ;
	
   // tReceiveSpline4.execute(pReceiveSpline);
   //tReceiveBall2015.execute(pReceiveSpline);
   
   	//	tPass.execute(pAttack);
	//	tKickoff.execute(pAttack) ;
   
		//tBackup0.execute(pBackup);
        //tDefendArc0.execute(pDefendArc);
       //        tDefendLine1.execute(pDefendL1);
	   // tDefendLine2.execute(pDefendL2);
	/*
	   //new defense 
	   tGoalOur1.execute(paramGoal);
	   tcoverleft2.execute(paramcoverleft);
       tcoverright0.execute(paramcoverright);
       defender2_4.execute(paramDefend2) ;
   */
   
   /*  
    // offense 1 with attack , receive , goalie , covergoal and defender
     tGoalOur0.execute(paramGoal) ;*/
	
	std::cout<<"Before execute"<<std::endl;
	if(loopcount++ > 14){
	//	tGoalOur0.execute(paramGoal);
	//tcover20151.execute(paramcover2015);
	//	dwDefend20152.execute(paramDWDefend2015);
	//	if(Vector2D<int>::dist(state.ballPos , state.homePos[3]) < Vector2D<int>::dist(state.ballPos , state.homePos[4]))
	//	tAttackSpline1.execute(pAttack) ;
	//  tReceiveSpline1.execute(pReceiveSpline) ;
		
/*		else
		{
			tAttackSpline4.execute(pAttack) ;
			tReceiveSpline3.execute(pReceiveSpline) ;
		}*/
		loopcount = loopcount%1000 + 15;
	 }
	else{
	
		tVelocity1.execute(pVelocity);	
	}
	std::cout<<"After execute"<<std::endl;
   /*
	// offense 2 with attack , supportattack , receive , goalie , covergoal 
	 
   */	
   
   /*
       defense2 with goalie , two covergoal , defender2 , attack 
	    
   */
   
          
		  if(loopcount++ > 14){
			 // tGoalOur3.execute(paramGoal);
			 // tcover20150.execute(paramcover2015);
			 // dwDefend20151.execute(paramDWDefend2015);
			//	sppoint.executeSkill(SkillSet::SplineGoToPoint , params1) ;
			//	params1.SplineGoToPointP.initTraj = 0;
		//	if(abs(state.ballPos.x)>50 && abs(state.ballPos.y)>50){
			//sball.executeSkill(SkillSet::SplineInterceptBall , params4) ;
			//params4.SplineInterceptBallP.initTraj = 0;
		//	}
			//if(Vector2D<int>::dist(state.ballPos, state.homePos[0]) < Vector2D<int>::dist(state.ballPos, state.homePos[4])){
       //     tAttackSpline2.execute(pAttack) ;
	   //  tSupportAttack0.execute(pAttack) ;
		   /*
			if(Vector2D<int>::dist(state.ballPos , state.homePos[1]) < Vector2D<int>::dist(state.ballPos , state.homePos[0]))
			{ tAttackSpline1.execute(pAttack) ;
			  tSupportAttack0.execute(pAttack) ;
			}
			else
			{
			  	tAttackSpline0.execute(pAttack) ;
			  tSupportAttack1.execute(pAttack) ;
			}
			*/
			//loopcount = loopcount%1000 + 14;
		}
		else{
	//		tVelocity4.execute(pVelocity);		
		}
		
		std::cout<<"Before write"<<std::endl;
        SkillSet::comm->writeCombinedPacket();   
		std::cout<<"After write"<<std::endl;    
   //**************************************** END OF TESTING ************************************************  
	 }
     else
      {
        printf("OFF!\n");
        tS0.execute(paramStop);
        tS1.execute(paramStop);
        tS2.execute(paramStop);
      }
    //      printf("tIMER US = %d\n", timer.stopus() );
      //moprintf("Ball Pos: %d %d %f\n",state.ballPos.x,state.ballPos.y,state.homeAngle[2]);
         usleep(16000);  // Adding sleep to this thread of execution to prevent CPU hogging
      
    }
	fclose(f1);
    vThread.stop();
    Util::Logger::toStdOut("Exiting process");
//	myfile.close();
    return;
  }
};