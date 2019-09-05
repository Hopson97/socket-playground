#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include "common.h"

namespace net {
    class ClientManager final {
      public:
        ClientManager(const sf::IpAddress &host, Port port);

        bool isConnected() const;

      private:
        void send(sf::Packet &packet);

        sf::UdpSocket m_socket;
        sf::IpAddress m_hostIp;
        Port m_hostPort;

        ClientId m_clientid;

        bool m_isConnected = false;
    };
} // namespace net