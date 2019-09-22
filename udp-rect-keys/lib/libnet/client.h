#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <array>
#include <functional>

#include "event.h"

namespace net {
    class Client final {
      public:
        Client(const sf::IpAddress &serverAddress, Port serverPort);

        bool send(sf::Packet &packet);
        bool receive(sf::Packet &packet);

      private:
        sf::UdpSocket m_socket;

        const sf::IpAddress m_serverAddress;
        const Port m_serverPort;
        ClientId m_id;

        // Used for receiving packets
        sf::IpAddress m_recievedIp;
        Port m_recievedPort;
    };
} // namespace net
