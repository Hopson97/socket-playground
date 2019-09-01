#pragma once

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>

namespace net {
    class Server final {
        public:
            Server();

        private:
            sf::UdpSocket m_socket;

            sf::Clock m_interalClock;
    };
}