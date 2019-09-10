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
        Player() { sprite.setSize({64, 256}); }

        sf::RectangleShape sprite;
        float velocity;
    };
    void pollWindowEvents();

    sf::RenderWindow m_window;
    ClientManager m_client;

    Player m_player;
    Player m_enemy;

    Keyboard m_keyboard;
};