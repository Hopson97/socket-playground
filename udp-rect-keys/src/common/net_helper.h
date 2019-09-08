#pragma once

#include <SFML/Network/Packet.hpp>
#include "commands.h"
#include "constants.h"

struct RecievedCommandInfo {
    Command command;
    ClientId id;
    sf::IpAddress sender;
    Port senderPort;
};

sf::Packet makePacket(Command command, ClientId clientId);

RecievedCommandInfo recievePacket(sf::UdpSocket &socket, sf::Packet &packet);

