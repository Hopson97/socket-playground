#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <cstdint>
#include <tuple>

using Port = uint16_t;
using ClientId = uint8_t;

constexpr Port PORT = 42124;
constexpr std::size_t MAX_CLIENTS = 4;

enum class MessageType {
    ConnectionRequest,
    ConnectionAccept,
    ConnectionRefuse,

    PlayerJoin,
    PlayerPosition,

    KeepAlive
};

MessageType getMessageType(sf::Packet &packet);

std::tuple<bool, sf::Packet, sf::IpAddress, Port>
getIncomingPacket(sf::UdpSocket &);