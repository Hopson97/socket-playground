#include "server.h"

#include <SFML/Network/Packet.hpp>

#include <iostream>
#include <thread>

#include <sabre/event.h>
#include <sabre/packet_factory.h>

#include "../common/commands.h"

Server::Server()
    : m_server(
          [this](const sabre::Event::Details &details) {
              auto &player = m_players[static_cast<std::size_t>(details.id)];
              player.connected = true;
              player.id = details.id;
              std::cout << "Player connected!\n";
              std::cout << "ID: " << (int)player.id << std::endl;
          },
          [this](const sabre::Event::Details &details) {
              const auto id = details.id;
              m_players[static_cast<std::size_t>(id)].connected = false;
          })
{
}

void Server::run()
{
    while (m_isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        m_server.whileTicking<Command>(
            [this](const sabre::Event::Details &details, sf::Packet &packet,
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

void Server::handlePlayerPosition(sabre::ClientId id, sf::Packet &packet)
{
    auto &player = m_players[static_cast<std::size_t>(id)];
    float x;
    float y;
    packet >> x >> y;
    player.rect.left = x;
    player.rect.top = y;
}

void Server::handleRequestPlayerPositions(sabre::ClientId requesterId)
{
    for (const auto &player : m_players) {
        if (player.connected && (requesterId != player.id)) {
            auto packet = sabre::makePacket(player.id, Command::PlayerPosition);
            packet << player.rect.left << player.rect.top;
            m_server.sendPacketToPeer(requesterId, packet);
        }
    }
}