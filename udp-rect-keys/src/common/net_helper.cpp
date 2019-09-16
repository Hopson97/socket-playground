#include "net_helper.h"

#include <SFML/Network/UdpSocket.hpp>

#include <iostream>

sf::Packet makePacket(Command command, ClientId clientId)
{
    sf::Packet packet;
    packet << static_cast<uint16_t>(command) << clientId;
    return packet;
}

bool isRecievePacket(sf::UdpSocket& socket, RecievedCommandInfo& info, sf::Packet& packet)
{
    if (socket.receive(packet, info.sender, info.senderPort) ==
        sf::Socket::Done) {
        packet >> info.command >> info.id;
        std::cout << (int)info.command << std::endl;
        return true;
    }
    return false;
}