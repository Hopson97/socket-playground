#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include "../common/constants.h"

class ClientManager final {
  public:
    ClientManager(const sf::IpAddress &host, Port port);

    void tick();

    bool isConnected() const;
    ClientId clientId() const { return m_clientid; }

    void send(sf::Packet &packet);

  private:
    sf::UdpSocket m_socket;
    sf::IpAddress m_hostIp;
    Port m_hostPort;

    ClientId m_clientid;

    bool m_isConnected = false;
};
