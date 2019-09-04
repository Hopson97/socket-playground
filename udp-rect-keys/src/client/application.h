#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <net/client.h>

class Application {
  public:
    Application();

    void run();

  private:
    sf::RenderWindow m_window;
    net::Client m_client;
};