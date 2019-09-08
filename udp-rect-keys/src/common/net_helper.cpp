#include "net_helper.h"

sf::Packet makePacket(Command command, ClientId clientId) {
    sf::Packet packet;
    packet << static_cast<uint16_t>(command) << clientId;
    return packet;
}