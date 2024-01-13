#include <cassert>
#include "comm.h"
#include "cs.hpp"
#include <cmath>
#define PI 3.141592653589793
using namespace Util;

namespace HAL
{
  bool Comm::initializedOnce = false;
  CS   Comm::commCS;

  Comm::Comm()
  {
    assert(initializedOnce == false);
    initializedOnce = true;
  }

  Comm::~Comm()
  { }  
  
  void Comm::addAdjustBot(int team , int id, int x, int y, float dir, int vx, int vy, int vz)
  {
    //grSim only functionality. not implemented in firacomm
  }
  
  void Comm::addAdjustBall(int x, int y, int vx, int vy, int vz)
  {
    //grSim only functionality. not implemented in firacomm
  }
  
  void Comm::addArrow(int to_x, int to_y, int from_x, int from_y, unsigned int color)
  {
    //grSim only functionality. not implemented in firacomm
  }
  
  std::list<Debug_Circle> Comm::getCircles() { 
    debug_cs->enter();
    std::list<Debug_Circle> temp = debug_circles;
    debug_cs->leave();
    return temp;
  }
  
  std::list<Debug_Line> Comm::getLines() {
    debug_cs->enter();
    std::list<Debug_Line> temp = debug_lines;
    debug_cs->leave();
    return temp;
  }
} // namespace HAL
