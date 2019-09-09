#include "net_helper.h"

#include <SFML/Network/UdpSocket.hpp>

#include <iostream>

sf::Packet makePacket(Command command, ClientId clientId)
{
    sf::Packet packet;
    packet << static_cast<uint16_t>(command) << clientId;
    return packet;
}

RecievedCommandInfo recievePacket(sf::UdpSocket &socket, sf::Packet &packet)
{
    RecievedCommandInfo info;
    if (socket.receive(packet, info.sender, info.senderPort) !=
        sf::Socket::Done) {
        std::cout << "Could not recieve packet\n";
    }
    packet >> info.command >> info.id;
    return info;
}