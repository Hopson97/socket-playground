#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <libnet/server.h>

#include <array>

#include "../common/constants.h"

struct RecievedCommandInfo;

class Server final {
    struct Player {
        sf::FloatRect rect;
        ClientId id;
    };

  public:
    Server();

    void run();

  private:
    net::Server m_server;

    struct ClientConnection {
        ClientId id;
        sf::Time lastUpdate;
        sf::IpAddress address;
        Port port;
        bool isConnected = false;
        sf::FloatRect playerBounds;

        void init(const RecievedCommandInfo &info, ClientId id);
    };

    void handlePacket(const RecievedCommandInfo &info, sf::Packet &packet);

    void handleIncomingConection(const RecievedCommandInfo &info);

    void handlePlayerPosition(const RecievedCommandInfo &info,
                              sf::Packet &packet);

    void handleRequestPlayerPositions(const RecievedCommandInfo &info);

    std::size_t emptySlot();

    sf::UdpSocket m_socket;
    sf::Clock m_interalClock;
    std::array<ClientConnection, CLIENT_COUNT> m_clientSlots;
    bool m_isRunning = true;
};