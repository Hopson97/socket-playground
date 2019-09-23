#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <sabre/server.h>

#include <array>

struct receivedCommandInfo;

class Server final {
    struct Player {
        sf::FloatRect rect;
        sabre::ClientId id;
        bool connected = false;
    };

  public:
    Server();

    void run();

  private:
    void handlePlayerPosition(sabre::ClientId id, sf::Packet &packet);
    void handleRequestPlayerPositions(sabre::ClientId id);

    sabre::Server m_server;
    std::array<Player, sabre::Server::MAX_CONNECTIONS> m_players;

    bool m_isRunning = true;
};