#include "commands.h"

#include <SFML/Network/UdpSocket.hpp>
#include <iostream>

sf::Packet &operator<<(sf::Packet &packet, Command command)
{
    packet << static_cast<uint16_t>(command);
    return packet;
}

sf::Packet &operator>>(sf::Packet &packet, Command &command)
{
    uint16_t commandId;
    packet >> commandId;
    command = static_cast<Command>(commandId);
    return packet;
}