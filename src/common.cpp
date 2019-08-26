#include "common.h"

MessageType getMessageType(sf::Packet &packet)
{
    uint8_t type;
    packet >> type;
    return static_cast<MessageType>(type);
}