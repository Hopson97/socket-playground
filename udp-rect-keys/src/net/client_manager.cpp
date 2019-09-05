#include "client_manager.h"

#include "commands.h"
#include <SFML/Network/Packet.hpp>
#include <iostream>

namespace net {
    ClientManager::ClientManager(const sf::IpAddress &host, Port port)
        : m_hostIp(host)
        , m_hostPort(port)
    {
        // Send connection request
        sf::Packet packet;
        packet << Command::RequestConnection
               << sf::IpAddress::getLocalAddress().toString();
        send(packet);

        // Get response
        packet.clear();
        auto recieved = recieveCommand(m_socket, packet);
        switch (recieved.command) {
            case Command::AcceptConnection:
                m_clientid = recieved.id;
                m_isConnected = true;
                std::cout << "Connection success\n";
                break;

            case Command::RejectConnection:
                std::cout << "Connection refused\n";
                break;
        }
    }

    void ClientManager::send(sf::Packet &packet)
    {
        if (m_socket.send(packet, m_hostIp, m_hostPort) != sf::Socket::Done) {
            std::cout << "Failed to send packet to host\n";
        }
    }

    bool ClientManager::isConnected() const {
        return m_isConnected;
    }
} // namespace net