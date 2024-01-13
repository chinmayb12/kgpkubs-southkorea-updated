#ifndef SSLVISION_CLIENT_H
#define SSLVISION_CLIENT_H

#include "comdef.h"
#include "server.h"
#include "RobocupCommDefs.h"

namespace RobocupLib
{
  class SSLVisionClient
  {
  private:
    SSL_WrapperPacket        recPacket;
    SimuServer               *Game;
        
  public:
    SSLVisionClient();

    ~SSLVisionClient();

    void getVisionInfo(VisionInfo *visionInfo);
  };
} /* RobocupLib */

#endif  /* SSLVISION_CLIENT_H */