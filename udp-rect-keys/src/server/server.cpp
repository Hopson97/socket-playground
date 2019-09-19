#include "server.h"
#include "../common/net_helper.h"

#include <SFML/Network/Packet.hpp>

#include <iostream>
#include <thread>

Server::Server()
{
    m_socket.bind(PORT);
    m_socket.setBlocking(false);
    for (int i = 0; i < (int)m_clientSlots.size(); i++) {
        m_clientSlots[i].id = i;
    }
    std::cout << "Server has been set up\n";
}

void Server::run()
{
    m_isRunning = true;
    while (m_isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        RecievedCommandInfo info;
        sf::Packet packet;
        while (isRecievePacket(m_socket, info, packet)) {
            handlePacket(info, packet);
        }
        for (auto &client : m_clientSlots) {
            if (client.isConnected &&
                m_interalClock.getElapsedTime() - client.lastUpdate >
                    sf::seconds(1)) {
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
            case Command::PlayerPosition:
                handlePlayerPosition(info, packet);
                break;

            case Command::GetPlayerPositions:
                handleRequestPlayerPositions(info);

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
        sf::Packet response = makePacket(Command::AcceptConnection,
                                         static_cast<ClientId>(slotNumber));
        auto &slot = m_clientSlots[slotNumber];
        slot.init(info, slotNumber);
        slot.lastUpdate = m_interalClock.getElapsedTime();

        if (m_socket.send(response, info.sender, info.senderPort) !=
            sf::Socket::Done) {
            m_clientSlots[slotNumber].isConnected = false;
        }
    }
}

void Server::handlePlayerPosition(const RecievedCommandInfo &info,
                                  sf::Packet &packet)
{
    auto &slot = m_clientSlots[static_cast<std::size_t>(info.id)];
    float x;
    float y;
    packet >> x >> y;
    slot.playerBounds.left = x;
    slot.playerBounds.top = y;
}

void Server::handleRequestPlayerPositions(const RecievedCommandInfo &info)
{
    auto &slot = m_clientSlots[static_cast<std::size_t>(info.id)];

    for (const auto &player : m_clientSlots) {
        if (player.isConnected) {
            auto packet = makePacket(Command::PlayerPosition, player.id);
            packet << player.playerBounds.left << player.playerBounds.top;
            if (m_socket.send(packet, slot.address, slot.port) !=
                sf::Socket::Done) {
                std::cout << "Failed to send packet to host\n";
            }
        }
    }
}

std::size_t Server::emptySlot()
{
    for (int i = 0; i < (int)m_clientSlots.size(); i++) {
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