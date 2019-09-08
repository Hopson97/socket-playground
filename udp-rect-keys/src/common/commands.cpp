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

RecievedCommandInfo recieveCommand(sf::UdpSocket &socket, sf::Packet &packet)
{
    auto info = recieveNoIdCommand(socket, packet);
    packet >> info.id;
    return info;
}

RecievedCommandInfo recieveNoIdCommand(sf::UdpSocket &socket,
                                       sf::Packet &packet)
{
    RecievedCommandInfo info;
    if (socket.receive(packet, info.sender, info.senderPort) !=
        sf::Socket::Done) {
        std::cout << "Could not recieve packet\n";
    }
    packet >> info.command;
    return info;
}
