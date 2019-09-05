#pragma once

#include "../net/client_manager.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Application {
  public:
    Application();

    void run();

  private:
    void pollWindowEvents();

    sf::RenderWindow m_window;
    net::ClientManager m_client;
};