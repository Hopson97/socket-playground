#pragma once

namespace sf {
    class Packet;
    class UdpSocket;
} // namespace sf

#include <cstdint>
#include <SFML/Network/IpAddress.hpp>

namespace net {
    using Port = std::uint16_t;
    using ClientId = std::uint8_t;

    struct Event {
        enum class EventType : uint16_t {
            Connect,
            DataRecieve,
            Disconnect,
            KeepAlive
        };

        struct RequestDetails {
            ClientId senderId;
            sf::IpAddress senderIp;
            Port senderPort;
        };

        friend sf::Packet &operator<<(sf::Packet &packet, EventType &type);
        friend sf::Packet &operator>>(sf::Packet &packet, EventType &type);

        EventType type;
        RequestDetails details;
    };

    bool receiveNetEvent(sf::UdpSocket &socket, sf::Packet &packet,
                         Event &event);
} // namespace net
