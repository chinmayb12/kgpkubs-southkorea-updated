#include "publisher.h"
#include <string>
Publisher::Publisher(Logger &logger, std::string &host, std::string &port, std::string &interface):
    bcast(logger, host, port, interface)
{
}

void Publisher::publish(StrategyPacket &p)
{
    std::string packet = p.SerializeAsString();
    // Send the packet.
    bcast.send(packet.data(), packet.size());
}
