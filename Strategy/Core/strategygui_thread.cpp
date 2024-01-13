#include "strategygui_thread.hpp"
#include "cs.hpp"
#include "Socket.h"
#include "logger.h"
#include "config.h"
#include <exception>
#include <cstring>
using namespace std;
using namespace Util;
  
namespace Strategy
{
  StrategyGUIThread* StrategyGUIThread::minstance = NULL;
  
  StrategyGUIThread::StrategyGUIThread(StrategyPacket* strPktSh, Util::CS* strCS):
    strPktSh(strPktSh),
    strCS(strCS)
  {
    try
    {
      sock = new UDPSocket(STRATEGY_GUI_MULTICAST_PORT);
      sock->joinGroup(STRATEGY_GUI_MULTICAST_ADDR);
    }
    catch (SocketException &e)
    {
      Logger::toStdOut("\nStrategy GUI thread could not be started. Do you want to continue anyways (y/n)? ");
      char resp = getchar();
      if (resp != 'y' && resp != 'Y')
      {
        fprintf(stderr, "%s", e.what());
        exit(EXIT_FAILURE);
      }
    }

    strPkt = new StrategyPacket();
    Logger::toStdOut("Strategy GUI connected...\n");
  }
  StrategyGUIThread& StrategyGUIThread::getInstance(StrategyPacket* strPktSh, Util::CS* strCS) {
    if(!minstance) {
      minstance = new StrategyGUIThread(strPktSh, strCS);
    } 
    return *minstance;
  }
  StrategyGUIThread::~StrategyGUIThread()
  {
    delete sock;
    delete strPkt;
  }
  void StrategyGUIThread::run() {
    while(true) {
      uint8_t buffer[65536];
      ushort port = STRATEGY_GUI_MULTICAST_PORT;
      std::string addr = STRATEGY_GUI_MULTICAST_ADDR;
      ssize_t len = sock->recvFrom(buffer, sizeof(buffer), addr, port);
      if (len < 0) {
        std::cerr << std::strerror(errno) << '\n';
        continue;
      }
      
      if(!strPktSh->ParseFromArray(buffer, len)) {
        std::cerr << "Protobuf parsing error!\n";
        continue;
      }
      
      cout << "Got valid packet: " << strPkt->which() << endl;
      sleep(16000);
      //sock->recvFrom
    }
  }
}

