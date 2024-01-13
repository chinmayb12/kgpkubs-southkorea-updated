#ifndef SERVER_H
#define SERVER_H

#include "comdef.h"
#include <stdio.h>
#include "netraw.h"
#include "config.h"
#include <string>
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"

class SimuServer
{
private:
  static const int MaxDataGramSize = 65536;
  char * in_buffer;
  int port;
  std::string address;
  std::string inter_face;
  Net::UDP multiCast;
  long int frameNo;
  int tcapture;
  SSL_DetectionFrame *dframe;
  SSL_DetectionBall *ball;
  SSL_DetectionRobot *bot[NUM_HOME_BOTS + NUM_AWAY_BOTS];

public:
  SimuServer(int port=Simulator::PORT, std::string address=Simulator::ADDRESS, std::string inter_face=Simulator::INTER_FACE);
  ~SimuServer();
  bool openMulticast(bool block=false);
  void closeMulticast();
  bool send(const SSL_WrapperPacket & packet);
  bool send(const SSL_DetectionFrame & frame);
  bool send(const SSL_GeometryData & geometry);
  bool receive(SSL_WrapperPacket &packet);
  void updateBot(int team,int botNo,float x,float y,float orientation);
  void updateBall(float x,float y);
  bool send_detection(double tcap,double tnow);
  //bool send_field_data();
};

#endif /* SERVER_H */
