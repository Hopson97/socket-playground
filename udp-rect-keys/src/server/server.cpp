#include "server.h"
#include "../common/net_helper.h"

#include <SFML/Network/Packet.hpp>

#include <iostream>

Server::Server()
{
    m_socket.bind(PORT);
    m_socket.setBlocking(false);
    for (int i = 0; i < m_clientSlots.size(); i++) {
        m_clientSlots[i].id = i;
    }
    std::cout << "Server has been set up\n";
}

void Server::run()
{
    m_isRunning = true;
    while (m_isRunning) {
        RecievedCommandInfo info;
        sf::Packet packet;
        while (m_socket.receive(packet, info.sender, info.senderPort) ==
               sf::Socket::Done) {
            packet >> info.command >> info.id;
            handlePacket(info, packet);
        }
        for (auto& client : m_clientSlots) {
            if (client.isConnected && m_interalClock.getElapsedTime() - client.lastUpdate > sf::seconds(1)) {
                client.isConnected = false;
                std::cout << (int)client.id << " has timed-out\n";
            }
        }

    }
}

void Server::handlePacket(const RecievedCommandInfo &info, sf::Packet &packet)
{
    if (info.command == Command::RequestConnection) {
        handleIncomingConection(info);
    }
    else {
        auto &client = m_clientSlots[info.id];
        client.lastUpdate = m_interalClock.getElapsedTime();
        switch (info.command) {
            default:
                break;
        }
    }
}

void Server::handleIncomingConection(const RecievedCommandInfo &info)
{
    sf::Packet response = makePacket(Command::RejectConnection, 0);
    auto slotNumber = emptySlot();
    std::cout << "Slot incoming: " << slotNumber << std::endl;
    if (slotNumber == 404) {
        sf::Packet response = makePacket(Command::RejectConnection, 0);
        response << "No free slots";
        if (m_socket.send(response, info.sender, info.senderPort) !=
            sf::Socket::Done) {
            m_clientSlots[slotNumber].isConnected = false;
        }
    }
    else {
        sf::Packet response = makePacket(Command::AcceptConnection, 0);
        auto &slot = m_clientSlots[slotNumber];
        slot.init(info, slotNumber);
        slot.lastUpdate = m_interalClock.getElapsedTime();

        if (m_socket.send(response, info.sender, info.senderPort) !=
            sf::Socket::Done) {
            m_clientSlots[slotNumber].isConnected = false;
        }
    }
}

std::size_t Server::emptySlot()
{
    for (int i = 0; i < m_clientSlots.size(); i++) {
        if (!m_clientSlots[i].isConnected) {
            return i;
        }
    }
    return 404;
}

void Server::ClientConnection::init(const RecievedCommandInfo &info,
                                    ClientId id)
{
    this->address = info.sender;
    this->port = info.senderPort;
    this->id = id;
    this->isConnected = true;
}