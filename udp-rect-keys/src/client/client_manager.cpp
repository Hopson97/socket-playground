#include "client_manager.h"

#include "../common/commands.h"
#include <SFML/Network/Packet.hpp>
#include <iostream>

#include "../common/net_helper.h"

ClientManager::ClientManager(const sf::IpAddress &host, Port port)
    : m_hostIp(host)
    , m_hostPort(port)
{
    // Send connection request
    auto packet = makePacket(Command::RequestConnection, 0);
    send(packet);

    // Get response
    packet.clear();
    auto recieved = recievePacket(m_socket, packet);
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
    m_socket.setBlocking(false);
}

void ClientManager::tick()
{
    auto packet = makePacket(Command::KeepAlive, m_clientid);
    send(packet);
}

void ClientManager::send(sf::Packet &packet)
{
    if (m_socket.send(packet, m_hostIp, m_hostPort) != sf::Socket::Done) {
        std::cout << "Failed to send packet to host\n";
    }
}

bool ClientManager::isConnected() const { return m_isConnected; }
