#include "mainheaders.hpp"
#include "thread.h"
using namespace Strategy;
#ifndef RUN_REFBOX
#define USE_FAKE_REFREE
#endif

class Worker:public Util::Thread {
public:
  bool &running;
  BeliefState &state;
  Util::CS*  writer_mutex;
  Util::CS*  writer_preference;
  Worker(bool &running_, BeliefState &state_, Util::CS* writer_mutex_, Util::CS* writer_preference_):
  running(running_),state(state_),writer_mutex(writer_mutex_),writer_preference(writer_preference_) {}
  void fake_refree(HAL::RefBoxCmd  &refBoxCmdSh) 
  {
    static int countdown = 0;
    if(state.pr_gameRunning) {
      const int ballx = state.ballPos.x;
      const int bally = state.ballPos.y;
      if(ballx < ForwardX(-HALF_FIELD_MAXX + 2*GOAL_DEPTH) && bally >= OUR_GOAL_MINY && bally <= OUR_GOAL_MAXY)
      {
        refBoxCmdSh.cmdCounter++;
        refBoxCmdSh.cmd = 'S';
        countdown = 1000;
        SkillSet::comm->addAdjustBall(0, 0, 0, 0, 0);
        refBoxCmdSh.goalsYellow++;
      } else if(ballx > ForwardX(HALF_FIELD_MAXX-2*GOAL_DEPTH) && bally >= OPP_GOAL_MINY && bally <= OPP_GOAL_MAXY)
      {
        refBoxCmdSh.cmdCounter++;
        refBoxCmdSh.cmd = 'S';
        countdown = 1000;
        SkillSet::comm->addAdjustBall(0, 0, 0, 0, 0);
        refBoxCmdSh.goalsBlue++;
      }
    } else if(countdown <= 0){
        refBoxCmdSh.cmdCounter++;
        refBoxCmdSh.cmd = 's';
        countdown = 1000;
    } else {
      countdown--;
    }
  }
  void run()
  {
	//printf("WORKERRRR\n");
	//assert(false);
    Util::CS loggerCS;
    Util::Logger logger("timelog.log", Util::Logger::Write, loggerCS);
    Kalman      kFilter;
    VisionThread vThread(&kFilter);
    vThread.start();

    PExec       pExec(&state);

    Util::Logger::toStdOut("Waiting for signal from Referee Box and SSLVision Client\n");

  unsigned char   refBoxCmdCounter = -1;
  HAL::RefBoxCmd  refBoxCmdSh;//  = new RefBoxCmd();
  #ifdef RUN_REFBOX
    Util::CS        refBoxCS;//     = new CS();
    RefBoxThread    refBoxThread = RefBoxThread::getInstance(&refBoxCmdSh, &refBoxCS);
    refBoxThread.start();
  #endif // RUN_REFBOX
  
    state.refreeUpdated = false;
	usleep(1000);
    while (running)
    {
      // std::cout<<"inside running"<<std::endl;
      writer_preference->enter();
      writer_mutex->enter();
      state.update();
      kFilter.update(state);   
      writer_mutex->leave();
      writer_preference->leave();
		
#ifdef RUN_REFBOX
      {
        // Critical Section protected by refBoxCS
        refBoxCS.enter();
        // Updating from Referee Box and the Game State
      //  printf("refBoxCmdSh.cmdCounter = %d\n",refBoxCmdSh.cmdCounter);
        if (refBoxCmdCounter != refBoxCmdSh.cmdCounter)
        {
          printf("GOT COMMAND FROM REFEREE!!!!\n");
          state.refreeUpdated = true;
          Util::Logger::toStdOut("Ref Box Updated to %d\n", refBoxCmdSh.cmdCounter);
          refBoxCmdCounter      = refBoxCmdSh.cmdCounter;
          state.updateStateFromRefree(refBoxCmdSh);
        }
		printf("%d\t%d\n",refBoxCmdSh.goalsBlue,refBoxCmdSh.goalsYellow);
        refBoxCS.leave();
      } // End of Critical Section protected by refBoxCS
#endif // RUN_REFBOX
#ifdef USE_FAKE_REFREE

      fake_refree(refBoxCmdSh);
		
      if (refBoxCmdCounter != refBoxCmdSh.cmdCounter)
      {        
		  
        state.refreeUpdated = true;
        refBoxCmdCounter      = refBoxCmdSh.cmdCounter;
        state.updateStateFromRefree(refBoxCmdSh);
      }
#endif
      if(state.refreeUpdated) 
        {		
          state.refreeUpdated = false;
          Util::Logger::toStdOut("Command From Refee.. Reselecting play..\n");
          pExec.selectPlay(NULL);
        }
        else if (pExec.playTerminated())
        {
          std::cout<<endl;
          std::cout<<"Play Terminated"<<std::endl;
          Util::Logger::toStdOut("*************Play terminated.Select new play\n*********************");
          pExec.evaluatePlay();
          pExec.selectPlay(NULL);
        }
        std::cout<<"Before in run"<<std::endl;
        pExec.executePlay();
        std::cout<<"After in run"<<std::endl;

      std::cout<<"writecombinedpacket"<<std::endl;
      SkillSet::comm->writeCombinedPacket();
  //   printf("%d\t%d\n",state.ourGoalCount,state.oppGoalCount);
      usleep(16000);  // Adding sleep to this thread of execution to prevent CPU hogging
    }
    vThread.stop();
    Util::Logger::toStdOut("Exiting process");
  }
};