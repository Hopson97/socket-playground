#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <libnet/server.h>

#include <array>

struct RecievedCommandInfo;

class Server final {
    struct Player {
        sf::FloatRect rect;
        net::ClientId id;
        bool connected = false;
    };

  public:
    Server();

    void run();

  private:
    void handlePlayerPosition(net::ClientId id, sf::Packet &packet);
    void handleRequestPlayerPositions(net::ClientId id);

    net::Server m_server;
    std::array<Player, net::Server::MAX_CONNECTIONS> m_players;

    bool m_isRunning = true;
};