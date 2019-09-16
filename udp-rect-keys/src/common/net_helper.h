#pragma once

#include "commands.h"
#include "constants.h"
#include <SFML/Network/Packet.hpp>

struct RecievedCommandInfo {
    Command command;
    ClientId id;
    sf::IpAddress sender;
    Port senderPort;
};

sf::Packet makePacket(Command command, ClientId clientId);

RecievedCommandInfo recievePacket(sf::UdpSocket &socket, sf::Packet &packet);

bool isRecievePacket(sf::UdpSocket& socket, RecievedCommandInfo& info, sf::Packet& packet);