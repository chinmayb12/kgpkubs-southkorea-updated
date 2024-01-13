#ifndef REF_BOX_THREAD_H
#define REF_BOX_THREAD_H

#include "thread.h"

// Forward Declarations
namespace Util
{
  class CS;
  class UDPSocket;
}

namespace HAL
{
  class RefBoxCmd;
}

namespace Strategy
{
  class RefBoxThread : public Util::Thread
  {
    friend class BeliefState;

  private:
    Util::UDPSocket* sock;
    HAL::RefBoxCmd*  refBoxCmdSh;
    HAL::RefBoxCmd*  refBoxCmd;
    Util::CS*        refBoxCS;
    static RefBoxThread  *minstance;
    
    /* Singleton Pattern class. */
    RefBoxThread(HAL::RefBoxCmd* refBoxCmdSh, Util::CS* refBoxCS);
    void run();

  public:
    static RefBoxThread& getInstance(HAL::RefBoxCmd* refBoxCmdSh, Util::CS* refBoxCS);
    ~RefBoxThread();
  };
}

#endif // REF_BOX_THREAD_H
