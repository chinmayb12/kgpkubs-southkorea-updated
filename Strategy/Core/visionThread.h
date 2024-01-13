#pragma once
#ifndef VISION_THREAD_H
#define VISION_THREAD_H

#include "thread.h"
#include "client.h"

// Forward Declarations
namespace HAL
{
  class VisionInfo;
}

namespace Util
{
  class CS;
}

namespace Strategy
{
  class Kalman;
}

namespace Strategy
{
  class VisionThread : public Util::Thread
  {

  private:
  
#ifdef VISION_COMM
    SSL_WrapperPacket recvPacket;
    static bool     instantiated;
    Kalman            *kFilter;
    Simulator::Client client;
    void run();

  public:
    VisionThread(Kalman* kFilter);
    ~VisionThread();
#else
    fira_message::sim_to_ref::Environment recvPacket;
    static bool instantiated;
    Kalman *kFilter;
    Simulator::Client client;
    void run();

  public:
    VisionThread(Kalman* kFilter);
    ~VisionThread();
#endif


  }; // class VisionThread
} // namespace Strategy

#endif  // VISION_THREAD_H
