#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "rbCommDef.h"
#include "refBoxThread.h"
#include "cs.hpp"
#include "Socket.h"
#include "logger.h"

using namespace std;
using namespace Util;
using namespace HAL;

namespace Strategy
{
  RefBoxThread* RefBoxThread::minstance = NULL;

  RefBoxThread::RefBoxThread(RefBoxCmd* refBoxCmdSh,
                             CS*        refBoxCS) :
    refBoxCmdSh(refBoxCmdSh),
    refBoxCS(refBoxCS)
  {
    // Connect to referee box
    try
    {
      sock = new UDPSocket(REF_BOX_MULTICAST_PORT);
      sock->joinGroup(REF_BOX_MULTICAST_ADDR);
    }
    catch (SocketException &e)
    {
      Logger::toStdOut("\nReferee box could not be started. Do you want to continue anyways (y/n)? ");
      char resp = getchar();
      if (resp != 'y' && resp != 'Y')
      {
        fprintf(stderr, "%s", e.what());
        exit(EXIT_FAILURE);
      }
    }

    refBoxCmd = new RefBoxCmd();
    Logger::toStdOut("Referee Box connected...\n");
  }
  
  RefBoxThread& RefBoxThread::getInstance(HAL::RefBoxCmd* refBoxCmdSh, Util::CS* refBoxCS) {
    if(!minstance) {
      minstance = new RefBoxThread(refBoxCmdSh, refBoxCS);
    } 
    return *minstance;
  }
  RefBoxThread::~RefBoxThread()
  {
    delete sock;
    delete refBoxCmd;
  }

  void RefBoxThread::run()
  {
    while (true)
    {
      try
      {
        unsigned short port = REF_BOX_MULTICAST_PORT;
        sock->recvFrom(refBoxCmd, sizeof(RefBoxCmd), REF_BOX_MULTICAST_ADDR, port);
        if (refBoxCmd->cmdCounter != refBoxCmdSh->cmdCounter)
        {
          assert(refBoxCmd->cmdCounter == (unsigned char)(refBoxCmdSh->cmdCounter + 1)); // Referee Box command missed

          refBoxCS->enter();
          memcpy(refBoxCmdSh, refBoxCmd, sizeof(RefBoxCmd));
          refBoxCS->leave();
#ifdef SHOW_REFBOX_LOG
          Logger::toStdOut("RFBox Counter = %d Cmd = %d GB = %d GY = %d Time Left = %d\n",
                           refBoxCmdSh->cmdCounter,
                           refBoxCmdSh->cmd,
                           refBoxCmdSh->goalsBlue,
                           refBoxCmdSh->goalsYellow,
                           refBoxCmdSh->timeRemaining);
#endif // SHOW_REFBOX_LOG
        }
      }
      catch (SocketException& e)
      {
        fprintf(stderr, "%s", e.what());
        exit(EXIT_FAILURE);
      }

      // Sleep for 5ms to prevent this thread from hogging the CPU
      sleep(5);
    }
  }
} // namespace Strategy
