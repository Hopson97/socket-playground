#pragma once

#include "client_manager.h"
#include "input/keyboard.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <SFML/Network/Packet.hpp>

struct RecievedCommandInfo;

class Application {
  public:
    Application();

    void run();

  private:
    constexpr static unsigned WINDOW_WIDTH = 1280;
    constexpr static unsigned WINDOW_HEIGHT = 720;
    struct Player {
        Player() { sprite.setSize({PLAYER_WIDTH, PLAYER_HEIGHT}); }

        sf::RectangleShape sprite;
        sf::Vector2f velocity;

        bool isConnected = false;
    };
    void pollWindowEvents();
    void handleIncomingPacket();

    void handleRecPlayerPosition(Player &player, sf::Packet &packet);

    sf::RenderWindow m_window;
    ClientManager m_client;

    Player &m_player;
    std::array<Player, CLIENT_COUNT> m_players;

    Keyboard m_keyboard;
};