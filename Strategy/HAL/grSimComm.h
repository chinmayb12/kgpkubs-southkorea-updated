/* This header file contains the definition of class SimComm  which inherits Comm class.
 * This class is used to  assign commands to the all the bots of the team for the 3D simulator.
 */
#pragma once
#ifndef SIM3D_COMM_H
#define SIM3D_COMM_H

#include <list>
#include <cmath>
#include "comm.h"
#include "netraw.h"
#include "sslDebug_Data.pb.h"

#define PI ((std::atan(1.0f)) * (4.0f))

namespace Util {
  class CS;
}
namespace HAL
{
  class GrSimComm : public Comm
  {
  private:
    Net::UDP                    udpsocket;
    Net::Address                _addr;
    
    bool reconnectUDP();
    void disconnectUDP();
	std::list<Debug_Circle> debug_circles_qt;
	std::list<Debug_Line> debug_lines_qt;
  public:
    GrSimComm();

    ~GrSimComm();
    virtual void addCircle(int x, int y, unsigned int radius, unsigned int color = 0xFFFFFFFF);
    virtual void addLine(int x1, int y1, int x2, int y2, unsigned int color = 0xFFFFFFFF);
    virtual void addAdjustBot(int team ,int id, int x, int y, float dir, int vx, int vy, int vz);
    virtual void addAdjustBall(int x, int y, int vx, int vy, int vz);
    virtual void addArrow(int to_x, int to_y, int from_x, int from_y, unsigned int color = 0xFFFFFFFF);
    void sendCommand(int   botID,
                     float v_l,
                     float v_r);
  };
}

#endif // SIM3D_COMM_H
