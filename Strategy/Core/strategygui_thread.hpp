#ifndef STRATEGY_GUI_THREAD
#define STRATEGY_GUI_THREAD

#include "thread.h"
#include "command-packet.pb.h"
// Forward Declarations
namespace Util
{
  class CS;
  class UDPSocket;
}

namespace Strategy
{
  class StrategyGUIThread : public Util::Thread
  {
    friend class BeliefState;
  public:
    static StrategyGUIThread& getInstance(StrategyPacket* strPktSh, Util::CS* strCS);
    ~StrategyGUIThread();
	void run();
  private:
    Util::UDPSocket* sock;
    StrategyPacket*  strPktSh;
    StrategyPacket*  strPkt;
    Util::CS*        strCS;
    static StrategyGUIThread *minstance;
    
    /* Singleton Pattern class. */
    StrategyGUIThread(StrategyPacket* strPktSh, Util::CS* strCS);
    
  };

}

#endif // STRATEGY_GUI_THREAD
