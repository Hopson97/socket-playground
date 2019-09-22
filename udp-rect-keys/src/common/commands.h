#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>

enum class Command : uint16_t {
    // Game play
    PlayerPosition,
    BallPosition,

    // Requests
    GetPlayerPositions
};

sf::Packet &operator>>(sf::Packet &packet, Command &command);
sf::Packet &operator<<(sf::Packet &packet, Command command);
