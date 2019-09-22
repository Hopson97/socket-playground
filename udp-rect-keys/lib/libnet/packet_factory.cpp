#include "packet_factory.h"

namespace net {
    sf::Packet makePacket(Event::EventType type)
    {
        sf::Packet packet;
        packet << type;
        return packet;
    }

    sf::Packet makePacket(Event::EventType type, ClientId id)
    {
        sf::Packet packet;
        packet << type << id;
        return packet;
    }
} // namespace net