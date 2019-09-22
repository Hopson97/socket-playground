#include "server.h"
#include "../common/net_helper.h"

#include <SFML/Network/Packet.hpp>

#include <iostream>
#include <thread>

#include <libnet/event.h>
#include <libnet/packet_factory.h>

Server::Server()
    : m_server(
          [this](const net::Event::Details &details) {
              auto &player = m_players[static_cast<std::size_t>(details.id)];
              player.connected = true;
              player.id = details.id;
              std::cout << "Player connected!\n";
              std::cout << "ID: " << (int)player.id << std::endl;
          },
          [this](const net::Event::Details &details) {
              const auto id = details.id;
              m_players[static_cast<std::size_t>(id)].connected = false;
          })
{
}

void Server::run()
{
    while (m_isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        m_server.whileRecievePacket<Command>(
            [this](const net::Event::Details &details, sf::Packet &packet,
                   Command command) {
                switch (command) {
                    case Command::PlayerPosition:
                        handlePlayerPosition(details.id, packet);
                        break;

                    case Command::GetPlayerPositions:
                        handleRequestPlayerPositions(details.id);
                        break;

                    default:
                        break;
                }
            });
    }
}

void Server::handlePlayerPosition(ClientId id, sf::Packet &packet)
{
    auto &player = m_players[static_cast<std::size_t>(id)];
    float x;
    float y;
    packet >> x >> y;
    player.rect.left = x;
    player.rect.top = y;
}

void Server::handleRequestPlayerPositions(ClientId requesterId)
{
    for (const auto &player : m_players) {
        if (player.connected && (requesterId != player.id)) {
            auto packet = net::makePacket(player.id, Command::PlayerPosition);
            packet << player.rect.left << player.rect.top;
            m_server.sendPacketToPeer(requesterId, packet);
        }
    }
}
/*
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
*/

/*
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
}*/