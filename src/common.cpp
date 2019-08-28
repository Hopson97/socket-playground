#include "common.h"

MessageType getMessageType(sf::Packet &packet)
{
    uint8_t type;
    packet >> type;
    return static_cast<MessageType>(type);
}

std::tuple<bool, sf::Packet, sf::IpAddress, Port>
getIncomingPacket(sf::UdpSocket &socket)
{
    sf::Packet packet;
    sf::IpAddress incomingIp;
    Port incomingPort;
    bool gotPacket =
        socket.receive(packet, incomingIp, incomingPort) == sf::Socket::Done;

    return {gotPacket, packet, incomingIp, incomingPort};
}