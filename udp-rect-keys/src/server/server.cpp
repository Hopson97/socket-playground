#include "server.h"

#include <SFML/Network/Packet.hpp>

#include <iostream>
#include <thread>

#include <libnet/event.h>
#include <libnet/packet_factory.h>

#include "../common/commands.h"

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

void Server::handlePlayerPosition(net::ClientId id, sf::Packet &packet)
{
    auto &player = m_players[static_cast<std::size_t>(id)];
    float x;
    float y;
    packet >> x >> y;
    player.rect.left = x;
    player.rect.top = y;
}

void Server::handleRequestPlayerPositions(net::ClientId requesterId)
{
    for (const auto &player : m_players) {
        if (player.connected && (requesterId != player.id)) {
            auto packet = net::makePacket(player.id, Command::PlayerPosition);
            packet << player.rect.left << player.rect.top;
            m_server.sendPacketToPeer(requesterId, packet);
        }
    }
}