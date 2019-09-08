#pragma once

#include "client_manager.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Application {
  public:
    Application();

    void run();

  private:
    void pollWindowEvents();

    sf::RenderWindow m_window;
    ClientManager m_client;
};