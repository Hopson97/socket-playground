#pragma once

#include <SFML/Network/Packet.hpp>

namespace net {
    enum class Command : uint16_t {
        // General commands
        RequestConnection,
        AcceptConnection,
        RejectConnection,
        KeepAlive,
        Disconnect,

        // To get number of commands
        CommandCount
    };

    sf::Packet &operator>>(sf::Packet &packet, Command command);
    sf::Packet &operator<<(sf::Packet &packet, Command command);
} // namespace net