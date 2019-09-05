#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>

#include "common.h"

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

    struct RecievedCommandInfo {
        Command command;
        ClientId id;
        sf::IpAddress sender;
        Port senderPort;
    };

    sf::Packet &operator>>(sf::Packet &packet, Command &command);
    sf::Packet &operator<<(sf::Packet &packet, Command command);

    RecievedCommandInfo recieveCommand(sf::UdpSocket &socket,
                                       sf::Packet &packet);
    RecievedCommandInfo recieveNoIdCommand(sf::UdpSocket &socket,
                                           sf::Packet &packet);
} // namespace net