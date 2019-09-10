#pragma once

#include "client_manager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class Application {
  public:
    Application();

    void run();

  private:
    void pollWindowEvents();

    sf::RenderWindow m_window;
    ClientManager m_client;

    struct Paddle {
      Paddle() {
        sprite.setSize({64, 256});
      }

      sf::RectangleShape sprite;
      float velocity;
    };
};