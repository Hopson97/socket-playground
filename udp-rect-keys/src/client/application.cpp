#include "application.h"

#include <SFML/Window/Event.hpp>
#include <iostream>
#include <sabre/packet_factory.h>

#include "../common/commands.h"

Application::Application()
    : m_client(sf::IpAddress::LocalHost, 54321,
               [this](const sabre::Event::Details &details) {
                   std::cout << details.senderIp.toString();
               },
               [this](const sabre::Event::Details &details) {
                   std::cout << details.senderIp.toString();
               })
    , m_player(m_players[m_client.getClientId()])
{
    m_player.sprite.setPosition(0, 0);
    m_player.isConnected = true;

    m_player.sprite.setOutlineColor(sf::Color::Red);
    m_player.sprite.setOutlineThickness(2);

    std::cout << "Client set up! ID: " << (int)m_client.getClientId() << '\n';
}

void Application::run()
{
     if (!m_client.connected()) {
        return;
    }
    m_window.create({WINDOW_WIDTH, WINDOW_HEIGHT}, "UDP Socket playground");
    m_window.setFramerateLimit(60);
    m_window.setKeyRepeatEnabled(false);

    constexpr unsigned TPS = 30; // ticks per seconds
    const sf::Time timePerUpdate = sf::seconds(1.0f / float(TPS));

    sf::Clock timer;
    sf::Clock netTimer;
    auto lastTime = sf::Time::Zero;
    auto lag = sf::Time::Zero;

    while (m_window.isOpen()) {
        // Get times
        auto time = timer.getElapsedTime();
        auto elapsed = time - lastTime;
        lastTime = time;
        lag += elapsed;

        input();
        while (lag >= timePerUpdate) {
            lag -= timePerUpdate;
            update(timer, elapsed);
        }

        m_client.whileTicking<Command>(
            [this](const sabre::Event::Details &details, sf::Packet &packet,
                   Command command) {
                auto &player =
                    m_players[static_cast<sabre::ClientId>(details.id)];
                player.isConnected = true;
                switch (command) {
                    case Command::PlayerPosition:
                        handlePlayerPosition(player, packet);
                        break;

                    default:
                        break;
                }
            });

        if (netTimer.getElapsedTime().asMilliseconds() > 25) {
            auto packet = sabre::makePacket(m_client.getClientId(),
                                          Command::PlayerPosition);
            packet << m_player.sprite.getPosition().x
                   << m_player.sprite.getPosition().y;
            m_client.send(packet);

            netTimer.restart();

            packet = sabre::makePacket(m_client.getClientId(),
                                     Command::GetPlayerPositions);
            m_client.send(packet);
            netTimer.restart();
        }
        render();
    }
}

void Application::input()
{
    pollWindowEvents();

    // Input
    float speed = 0.15f;
    if (m_keyboard.isKeyDown(sf::Keyboard::Up)) {
        m_player.velocity.y += -speed;
    }
    else if (m_keyboard.isKeyDown(sf::Keyboard::Down)) {
        m_player.velocity.y += speed;
    }
    if (m_keyboard.isKeyDown(sf::Keyboard::Left)) {
        m_player.velocity.x += -speed;
    }
    else if (m_keyboard.isKeyDown(sf::Keyboard::Right)) {
        m_player.velocity.x += speed;
    }
}

void Application::update(sf::Clock &elapsed, sf::Time delta)
{
    (void)delta;
    (void)elapsed;

    m_player.sprite.move(m_player.velocity);
    m_player.velocity *= 0.95f;

    const float x = m_player.sprite.getPosition().x;
    const float y = m_player.sprite.getPosition().y;

    if (x + PLAYER_WIDTH > WINDOW_WIDTH) {
        m_player.sprite.setPosition(WINDOW_WIDTH - PLAYER_WIDTH - 1, y);
    }
    if (y + PLAYER_HEIGHT > WINDOW_HEIGHT) {
        m_player.sprite.setPosition(x, WINDOW_HEIGHT - PLAYER_HEIGHT - 1);
    }
    if (x < 0) {
        m_player.sprite.setPosition(1, y);
    }
    if (y < 0) {
        m_player.sprite.setPosition(x, 1);
    }

    for (auto &player : m_players) {
        if (&player == &m_player)
            continue;

        auto lerp = [](float a, float b, float t) {
            return (1 - t) * a + t * b;
        };
        player.lerpValue += delta.asSeconds() * 2;
        auto newX = lerp(player.sprite.getPosition().x, player.nextPosition.x,
                         player.lerpValue);
        auto newY = lerp(player.sprite.getPosition().y, player.nextPosition.y,
                         player.lerpValue);

        player.sprite.setPosition(newX, newY);
    }
}

void Application::render()
{

    m_window.clear();

    for (auto &player : m_players) {
        if (player.isConnected) {
            m_window.draw(player.sprite);
        }
    }

    m_window.display();
}

void Application::pollWindowEvents()
{
    sf::Event e;
    while (m_window.pollEvent(e)) {
        m_keyboard.update(e);
        switch (e.type) {
            case sf::Event::Closed:
                m_window.close();
                break;

            default:
                break;
        }
    }
}

void Application::handlePlayerPosition(Player &player, sf::Packet &packet)
{
    packet >> player.nextPosition.x >> player.nextPosition.y;
    player.lerpValue = 0;
}
