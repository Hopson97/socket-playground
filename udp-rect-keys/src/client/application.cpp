#include "application.h"

#include <SFML/Window/Event.hpp>

#include "../common/net_helper.h"

Application::Application()
    : m_client(sf::IpAddress::LocalHost, PORT)
    , m_player(m_players[m_client.clientId()])
{
    m_player.sprite.setPosition({20, 20});
    m_player.sprite.setFillColor(sf::Color::Red);

    if (m_client.isConnected()) {
        m_player.isConnected = true;
        auto packet =
            makePacket(Command::GetPlayerPositions, m_client.clientId());
        m_client.send(packet);
    }
}

void Application::run()
{
    if (!m_client.isConnected()) {
        return;
    }
    m_window.create({1280, 720}, "UDP Socket playground");
    m_window.setFramerateLimit(60);
    m_window.setKeyRepeatEnabled(false);

    sf::Clock timer;

    while (m_window.isOpen()) {
        pollWindowEvents();

        // Input
        if (m_keyboard.isKeyDown(sf::Keyboard::Up)) {
            m_player.velocity.y += -0.1;
        }
        else if (m_keyboard.isKeyDown(sf::Keyboard::Down)) {
            m_player.velocity.y += 0.1;
        }
        if (m_keyboard.isKeyDown(sf::Keyboard::Left)) {
            m_player.velocity.x += -0.1;
        }
        else if (m_keyboard.isKeyDown(sf::Keyboard::Right)) {
            m_player.velocity.x += 0.1;
        }
        // Update
        m_player.sprite.move(m_player.velocity);
        m_player.velocity.x *= 0.98;
        m_player.velocity.y *= 0.98;

        if (timer.getElapsedTime().asSeconds() > 0.5) {
            auto packet = makePacket(Command::KeepAlive, m_client.clientId());
            m_client.send(packet);

            packet = makePacket(Command::PlayerPosition, m_client.clientId());
            packet << m_player.sprite.getPosition().x
                   << m_player.sprite.getPosition().y;

            timer.restart();
        }
        
        //Recieve
        {
            RecievedCommandInfo info;
            sf::Packet packet;
            while (m_client.recievePacket(info, packet)) {
                
            }
        }

        // Draw
        m_window.clear();

        for (auto &player : m_players) {
            if (player.isConnected) {
                m_window.draw(player.sprite);
            }
        }

        m_window.display();
    }
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

// http://enet.bespin.org/Tutorial.html