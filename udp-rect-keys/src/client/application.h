#pragma once

#include "client_manager.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Application {
  public:
    Application();

    void run();

  private:
    struct Paddle {
        Paddle() { sprite.setSize({64, 256}); }

        sf::RectangleShape sprite;
        float velocity;
    };
    void pollWindowEvents();

    sf::RenderWindow m_window;
    ClientManager m_client;
};