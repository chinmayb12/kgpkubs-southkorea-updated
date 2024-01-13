#include "client.h"

namespace Simulator
{

#ifdef VISION_COMM
  Client::Client(int port, std::string address, std::string inter_face) :
    port(port),
    address(address),
    inter_face(inter_face),
    frameNo(1),
    tcapture(0),
    nframe(NULL),
    gdata(NULL)   
  {
    openMulticast();

    ball = dframe.add_balls();
    ball->set_area(BALL_AREA);
    ball->set_pixel_x((float) BALL_PIXEL_X);
    ball->set_pixel_y((float) BALL_PIXEL_Y);
    ball->set_confidence(1);
    ball->set_x(0);
    ball->set_y(0);

    for(int i = 0; i < BlueTeam::SIZE; ++i)
    {
      bot[i] = dframe.add_robots_blue();
      bot[i]->set_robot_id(i);
      bot[i]->set_pixel_x((float) BOT_PIXEL_X);
      bot[i]->set_pixel_y((float) BOT_PIXEL_Y);
      bot[i]->set_confidence(1);
      bot[i]->set_x(0);
      bot[i]->set_y(0);
      bot[i]->set_orientation(0);
    }

    for(int i = 0; i < YellowTeam::SIZE; ++i)
    {
      bot[i + BlueTeam::SIZE] = dframe.add_robots_yellow();
      bot[i + BlueTeam::SIZE]->set_robot_id(i);
      bot[i + BlueTeam::SIZE]->set_pixel_x((float) BOT_PIXEL_X);
      bot[i + BlueTeam::SIZE]->set_pixel_y((float) BOT_PIXEL_Y);
      bot[i + BlueTeam::SIZE]->set_confidence(1);
      bot[i + BlueTeam::SIZE]->set_x(0);
      bot[i + BlueTeam::SIZE]->set_y(0);
      bot[i + BlueTeam::SIZE]->set_orientation(0);
    }
  }

  Client::~Client()
  { }

  bool Client::openMulticast(bool block)
  {
    closeMulticast();
    if(!(multiCast.open(port, true, true, block)))
    {
      fprintf(stderr, "Opening port : %d failed\n", port);
      fflush(stderr);
      return false;
    }
    Net::Address _address, _interface;
    if(!(_address.setHost(address.c_str(), port)))
    {
      fprintf(stderr, "setHost(address,port) failed\n");
      fflush(stderr);
      return false;
    }
    if(inter_face.length() > 0)
    {
      if(!(_interface.setHost(inter_face.c_str(), port)))
      {
        fprintf(stderr, "setHost(interface,port) failed\n");
        fflush(stderr);
        return false;
      }
    }
    else
    {
      _interface.setAny();
    }
    if(!(multiCast.addMulticast(_address, _interface)))
    {
      fprintf(stderr, "adding Multicast failed\n");
      fflush(stderr);
      return false;
    }
    fprintf(stdout, "Vision UDP network successfully configured. (Multicast address: %s:%d)\n", Simulator::ADDRESS, Simulator::VISION_PORT);
    return true;
  }

  void Client::closeMulticast()
  {
    multiCast.close();
  }

  void Client::updateBot(TeamColor teamColor, int botID, float x, float y, float orientation)
  {
    if (teamColor == BLUE_TEAM)
    {
      bot[botID]->set_x(x);
      bot[botID]->set_y(y);
      bot[botID]->set_orientation(orientation);
    }
    else if (teamColor == YELLOW_TEAM)
    {
      bot[botID + BlueTeam::SIZE]->set_x(x);
      bot[botID + BlueTeam::SIZE]->set_y(y);
      bot[botID + BlueTeam::SIZE]->set_orientation(orientation);
    }
    else
    {
      assert(0); // Invalid Team Color
    }
  }

  void Client::updateBall(float x, float y)
  {
    ball->set_x(x);
    ball->set_y(y);
  }

  bool Client::receive(SSL_WrapperPacket& packet)
  {
    Net::Address src;
    int r = 0;
    r = multiCast.recv(in_buffer, MaxDataGramSize, src);
    if (r > 0)
    {
      fflush(stdout);
      return packet.ParseFromArray(in_buffer, r);
    }
    return false;
  }
#else 
  Client::Client(int port, std::string address, std::string inter_face) :
    port(port),
    address(address),
    inter_face(inter_face),
    frameNo(1),
    tcapture(0),
    nframe(NULL)   
  {
    openMulticast();

    ball = new fira_message::Ball;
    ball->set_x(0);
    ball->set_y(0);
    ball->set_z(1);
    std::cout<<"after ball"<<std::endl;
    dframe.set_allocated_ball(ball);

    std::cout<<"here"<<std::endl;
    for(int i = 0; i < BlueTeam::SIZE; ++i)
    {
      bot[i] = dframe.add_robots_blue();
      bot[i]->set_robot_id(i);
      bot[i]->set_x(0);
      bot[i]->set_y(0);
      bot[i]->set_orientation(0);
    }

    for(int i = 0; i < YellowTeam::SIZE; ++i)
    {
      bot[i + BlueTeam::SIZE] = dframe.add_robots_yellow();
      bot[i + BlueTeam::SIZE]->set_robot_id(i);
      bot[i + BlueTeam::SIZE]->set_x(0);
      bot[i + BlueTeam::SIZE]->set_y(0);
      bot[i + BlueTeam::SIZE]->set_orientation(0);
    }
    
  }

  Client::~Client()
  { }

  bool Client::openMulticast(bool block)
  {
    closeMulticast();
    if(!(multiCast.open(port, true, true, block)))
    {
      fprintf(stderr, "Opening port : %d failed\n", port);
      fflush(stderr);
      return false;
    }
    Net::Address _address, _interface;
    if(!(_address.setHost(address.c_str(), port)))
    {
      fprintf(stderr, "setHost(address,port) failed\n");
      fflush(stderr);
      return false;
    }
    if(inter_face.length() > 0)
    {
      if(!(_interface.setHost(inter_face.c_str(), port)))
      {
        fprintf(stderr, "setHost(interface,port) failed\n");
        fflush(stderr);
        return false;
      }
    }
    else
    {
      _interface.setAny();
    }
    if(!(multiCast.addMulticast(_address, _interface)))
    {
      fprintf(stderr, "adding Multicast failed\n");
      fflush(stderr);
      return false;
    }
    fprintf(stdout, "Vision UDP network successfully configured. (Multicast address: %s:%d)\n", Simulator::ADDRESS, Simulator::VISION_PORT);
    return true;
  }

  void Client::closeMulticast()
  {
    multiCast.close();
  }

  void Client::updateBot(TeamColor teamColor, int botID, float x, float y, float orientation)
  {
    if (teamColor == BLUE_TEAM)
    {
      bot[botID]->set_x(x);
      bot[botID]->set_y(y);
      bot[botID]->set_orientation(orientation);
    }
    else if (teamColor == YELLOW_TEAM)
    {
      bot[botID + BlueTeam::SIZE]->set_x(x);
      bot[botID + BlueTeam::SIZE]->set_y(y);
      bot[botID + BlueTeam::SIZE]->set_orientation(orientation);
    }
    else
    {
      assert(0); // Invalid Team Color
    }
  }

  void Client::updateBall(float x, float y)
  {
    ball->set_x(x);
    ball->set_y(y);
  }

  bool Client::receive(fira_message::sim_to_ref::Environment& packet)
  {
    Net::Address src;
    int r = 0;
    r = multiCast.recv(in_buffer, MaxDataGramSize, src);
    if (r > 0)
    {
      fflush(stdout);
      return packet.ParseFromArray(in_buffer, r);
    }
    return false;
  }

#endif
} // namespace Simulator
