#pragma once

#include "client_manager.h"
#include "input/keyboard.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Application {
  public:
    Application();

    void run();

  private:
    struct Player {
        Player() { sprite.setSize({PLAYER_WIDTH, PLAYER_HEIGHT}); }

        sf::RectangleShape sprite;
        sf::Vector2f velocity;

        bool isConnected = false;
    };
    void pollWindowEvents();

    sf::RenderWindow m_window;
    ClientManager m_client;

    Player &m_player;
    std::array<Player, CLIENT_COUNT> m_players;

    Keyboard m_keyboard;
};