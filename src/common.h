#pragma once

#include <SFML/Network/Packet.hpp>
#include <cstdint>

using Port = uint16_t;

constexpr Port PORT = 42124;
constexpr uint8_t MAX_CLIENTS = 4;

enum class MessageType {
    ConnectionRequest,
    ConnectionAccept,
    ConnectionRefuse,

    PlayerPosition,
};

MessageType getMessageType(sf::Packet &packet);