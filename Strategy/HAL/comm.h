#pragma once
#ifndef COMM_H
#define COMM_H

#include "sslDebug_Data.pb.h"
#include <list>
#include "cs.hpp"
#include "common.pb.h"
// Forward Declarations
namespace Util
{
  class CS;
}

namespace HAL
{
  class Comm
  {
  private:
    // To ensure only one instance of Comm's derived class is created, the following flag is used
    static bool initializedOnce;
    
  protected:
    static Util::CS commCS;

  public:
    Comm();
    virtual ~Comm();
    std::list<Debug_Circle>     debug_circles;
    std::list<Debug_Line>       debug_lines;
    std::list<Debug_AdjustBot>  debug_adjustbot;
    std::list<Debug_AdjustBall> debug_adjustball;
    Util::CS*                   debug_cs;
    
    /* This function must be implemented in the derived classes.
     * Remember to guard the entire function body with the CS object commCS
     * otherwise the result will be undefined and communication protocols
     * may break down
     */
     //Making addCircle and addLine as purely virtual, since these debug symbols really 
    virtual void addCircle(int x, int y, unsigned int radius, unsigned int color = 0xFFFFFFFF)= 0;
    virtual void addLine(int x1, int y1, int x2, int y2, unsigned int color = 0xFFFFFFFF)=0;
    virtual void addAdjustBot(int team ,int id, int x, int y, float dir, int vx, int vy, int vz);
    virtual void addAdjustBall(int x, int y, int vx, int vy, int vz);
    virtual void addArrow(int to_x, int to_y, int from_x, int from_y, unsigned int color = 0xFFFFFFFF);
    virtual void sendCommand(int   botID,           // valid bot ID
                             float v_l,             // in mm/sec
                             float v_r             // in mm/sec
                             ) = 0;    // on(true) or off(false)
    virtual void writeCombinedPacket(){} //Implemented only in FIRAComm. Always writes to HOME_TEAM
    virtual void getSentData(int botid, int &vl, int &vr){} // Implemented only in FIRAComm, there too only used only for BOTLOG
	
    //Following provided for QtDebugger compatibiltity
    std::list<Debug_Circle> getCircles();
    std::list<Debug_Line> getLines();
    void clearDebugData() { debug_cs->enter(); debug_circles.clear(); debug_lines.clear(); debug_cs->leave();}
  }; // class Comm
} // namespace HAL

#endif // COMM_H
