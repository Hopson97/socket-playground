#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>

namespace net {
    class Client final {
      public:
        Client(const sf::IpAddress &host);

        private:

    };
} // namespace net

//http://enet.bespin.org/Tutorial.html