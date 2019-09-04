#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>

namespace net {
    template <typename I> class Client final {
      public:
        Client(const sf::IpAddress &host);
    };
} // namespace net