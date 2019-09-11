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
    RecievedCommandInfo info;
    if (m_socket.receive(packet, info.sender, info.senderPort) ==
        sf::Socket::Done) {
        packet >> info.command >> info.id;
        switch (info.command) {
            case Command::AcceptConnection:
                std::cout << "Connection accepted\n";
                m_clientid = info.id;
                std::cout << (int)m_clientid << '\n';
                m_isConnected = true;
                break;

            case Command::RejectConnection:
                std::cout << "Connection refused\n";
                break;
        }
        m_socket.setBlocking(false);
    }
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
