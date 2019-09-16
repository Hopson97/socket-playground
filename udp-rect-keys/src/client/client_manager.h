#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include "../common/constants.h"

struct RecievedCommandInfo;

class ClientManager final {
  public:
    ClientManager(const sf::IpAddress &host, Port port);

    bool isConnected() const;
    ClientId clientId() const { return m_clientid; }

    void send(sf::Packet &packet);

    bool recievePacket(RecievedCommandInfo& info, sf::Packet& packet);

  private:
    sf::UdpSocket m_socket;
    sf::IpAddress m_hostIp;
    Port m_hostPort;

    ClientId m_clientid;

    bool m_isConnected = false;
};
