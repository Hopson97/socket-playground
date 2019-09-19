#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <array>

#include "event.h"

struct Event;

namespace net {
    class Server final {
        static constexpr std::size_t MAX_CONNECTIONS = 4;

        struct Client {
            sf::IpAddress address;
            Port port;
            sf::Time lastUpdate;
        };

      public:
        Server();

        bool recievePacket(Event &event);

      private:
        void handleIncomingConnection(const Event &event);
        std::size_t emptySlot() const;

        std::array<Client, MAX_CONNECTIONS> m_clients;
        std::array<bool, MAX_CONNECTIONS> m_clientConnected;
        std::size_t currentConnections = 0;

        sf::UdpSocket m_socket;
        sf::Clock m_clock;
    };
} // namespace net