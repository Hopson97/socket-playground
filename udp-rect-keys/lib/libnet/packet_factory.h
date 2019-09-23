#pragma once

#include <SFML/Network/Packet.hpp>

#include "event.h"

namespace net {
    sf::Packet makePacket(Event::EventType event);
    sf::Packet makePacket(Event::EventType event, ClientId id);

    template <typename T> sf::Packet makePacket(ClientId id, T command)
    {
        sf::Packet packet;
        packet << Event::EventType::Data << id << command;
        return packet;
    }
} // namespace net
