#include <cstdio>
#include "comdef.h"
#include "grSimComm.h"
#include "cs.hpp"
#include "logger.h"
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "grSim_Replacement.pb.h"
#include "sslDebug_Data.pb.h"
#include <iostream>
#define D ((2*MAX_BOT_SPEED)/(MAX_BOT_OMEGA))
using namespace std;
using namespace Util;
namespace HAL
{
  GrSimComm::GrSimComm()
  {
    debug_cs = new CS();
    if (reconnectUDP())
    {
      for (int i = 0; i < Strategy::HomeTeam::SIZE; ++i)
      {
        sendCommand(i, 0, 0);
      }
      Logger::toStdOut("Robot command listen UDP network successfully configured. (Multicast address: %s:%d)\n", 
                        Simulator::ADDRESS, Simulator::COMMAND_PORT);
    }
    else
    {
      Logger::abort("Error : Could not configure the Robot command UDP network\n");
    }
  }
  void GrSimComm::addCircle(int x, int y, unsigned int radius, unsigned int color)
  {
    Debug_Circle circle;
    circle.set_x(x);
    circle.set_y(y);
    circle.set_radius(radius);
    circle.set_color(color);
    debug_cs->enter();
    debug_circles.push_back(circle);
    debug_cs->leave();
  }
  void GrSimComm::addLine(int x1, int y1, int x2, int y2, unsigned int color)
  {
    Debug_Line line;
    line.set_x1(x1);
    line.set_y1(y1);
    line.set_x2(x2);
    line.set_y2(y2);
    line.set_color(color);
    debug_cs->enter();
    debug_lines.push_back(line);
    debug_cs->leave();
  }
  void GrSimComm::addAdjustBot(int team , int id, int x, int y, float dir, int vx, int vy, int vz)
  {
    printf("adjust %d %d %d\n", id, x, y);
    Debug_AdjustBot botpos;
    botpos.set_x(x);
    botpos.set_y(y);
    botpos.set_vx(vx);
    botpos.set_vy(vy);
    botpos.set_vz(vz);
    botpos.set_team(team);
    botpos.set_id(id);
    botpos.set_dir(dir);
    debug_cs->enter();
    debug_adjustbot.push_back(botpos);
    debug_cs->leave();
  }
  void GrSimComm::addAdjustBall(int x, int y, int vx, int vy, int vz)
  {
    Debug_AdjustBall ballpos;
    ballpos.set_x(x);
    ballpos.set_y(y);
    ballpos.set_vx(vx);
    ballpos.set_vy(vy);
    ballpos.set_vz(vz);
    debug_cs->enter();
    debug_adjustball.push_back(ballpos);
    debug_cs->leave();
  }
  void GrSimComm::addArrow(int to_x, int to_y, int from_x, int from_y, unsigned int color)
  {
    addLine(to_x, to_y, from_x, from_y, color);
    float theta = atan2(from_y - to_y, from_x - to_x);
    float theta1 = theta - PI/6;
    float theta2 = theta + PI/6;
    addLine(to_x, to_y, to_x + 40*cos(theta1), to_y + 40*sin(theta1));
    addLine(to_x, to_y, to_x + 40*cos(theta2), to_y + 40*sin(theta2));
    addLine(to_x + 40*cos(theta1), to_y + 40*sin(theta1), to_x + 40*cos(theta2), to_y + 40*sin(theta2));
  }
  bool GrSimComm::reconnectUDP()
  {
    udpsocket.close();
    bool flag = _addr.setHost(Simulator::ADDRESS, Simulator::COMMAND_PORT);
    return (udpsocket.open(Simulator::COMMAND_PORT, false, false, false) && flag);
  }
  void GrSimComm::disconnectUDP()
  {
    udpsocket.close();
  }
  GrSimComm::~GrSimComm()
  {
    disconnectUDP();
  }
  void GrSimComm::sendCommand(int   botID,
                              float v_l,
                              float v_r)
  {
    commCS.enter();
    grSim_Packet packet;
    packet.mutable_commands()->set_isteamyellow(Strategy::HomeTeam::COLOR == Simulator::YELLOW_TEAM);
    packet.mutable_commands()->set_timestamp(0.0);
    grSim_Robot_Command* command = packet.mutable_commands()->add_robot_commands();
    command->set_id(botID);
    command->set_wheelsspeed(false);
    command->set_veltangent(((v_l + v_r) / 2) / 1000.0f); // Unit of length in Strategy used is mm but it is m in grSim
    command->set_velnormal(0 / 1000.0f); // Unit of length in Strategy used is mm but it is m in grSim
    command->set_velangular((v_r - v_l) / D);
    command->set_kickspeedx(0);
    command->set_kickspeedz(0);         // Chip-kicker disabled
    command->set_spinner(0);

    // Adding debug info to the command packet
    debug_cs->enter();
    debug_circles_qt = debug_circles;
    debug_lines_qt = debug_lines;
    while (debug_circles.empty() == false)
    {
      Debug_Circle circle = debug_circles.front();
      debug_circles.pop_front();
      packet.mutable_debuginfo()->add_circle()->CopyFrom(circle);
    }

    while (debug_lines.empty() == false)
    {
      Debug_Line line = debug_lines.front();
      debug_lines.pop_front();
      packet.mutable_debuginfo()->add_line()->CopyFrom(line);
    }
    while(!debug_adjustball.empty())
    {
      Debug_AdjustBall ballpos = debug_adjustball.front();
      packet.mutable_debuginfo()->add_ballpos()->CopyFrom(ballpos);
      debug_adjustball.pop_front();
    }
    while(!debug_adjustbot.empty())
    {
      Debug_AdjustBot botpos = debug_adjustbot.front();
      packet.mutable_debuginfo()->add_botpos()->CopyFrom(botpos);
      debug_adjustbot.pop_front();
    }
    debug_cs->leave();
    std::string s;
    packet.SerializeToString(&s);
    udpsocket.send((void*)s.c_str(), s.length(), _addr);
    commCS.leave();
  }
  
} // namespace HAL
