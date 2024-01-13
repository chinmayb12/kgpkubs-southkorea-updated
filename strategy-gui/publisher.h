#ifndef PUBLISHER_H
#define PUBLISHER_H
#include "noncopyable.h"
#include "udpbroadcast.h"
#include "logger.h"
#include "command-packet.pb.h"
class Publisher : public NonCopyable
{
public:
    Publisher(Logger &logger, std::string &host, std::string &port, std::string &interface);
    void publish(StrategyPacket &p);
private:
    UDPBroadcast bcast;
};

#endif // PUBLISHER_H
