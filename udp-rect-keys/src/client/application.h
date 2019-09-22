#pragma once

#include "client_manager.h"
#include "input/keyboard.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <SFML/Network/Packet.hpp>

#include <libnet/client.h>

struct RecievedCommandInfo;

class Application {
  public:
    Application();

    void run();

  private:
    constexpr static unsigned WINDOW_WIDTH = 400;
    constexpr static unsigned WINDOW_HEIGHT = 200;

    struct Player {
        Player() { sprite.setSize({PLAYER_WIDTH, PLAYER_HEIGHT}); }

        sf::RectangleShape sprite;
        sf::Vector2f velocity;

        sf::Vector2f nextPosition;
        float lerpValue;

        bool isConnected = false;
    };
    void input();
    void update(sf::Clock &elapsed, sf::Time delta);
    void render();

    void pollWindowEvents();
    /*
    void handleIncomingPacket();
*/
    void handlePlayerPosition(Player &player, sf::Packet &packet);

    /*
        ClientManager m_client;
    */
    Player &m_player;
    std::array<Player, CLIENT_COUNT> m_players;

    sf::RenderWindow m_window;
    Keyboard m_keyboard;
    net::Client m_client;
};