#include "application.h"

#include <SFML/Window/Event.hpp>

Application::Application()
    : m_client(sf::IpAddress::LocalHost, PORT)
{
    m_player.sprite.setPosition({20, 20});
    m_enemy.sprite.setPosition({1096, 20});
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
            m_player.velocity += -0.1;
        }
        else if (m_keyboard.isKeyDown(sf::Keyboard::Down)) {
            m_player.velocity += 0.1;
        }

        // Update
        m_player.sprite.move({0, m_player.velocity});
        m_player.velocity *= 0.98;

        if (timer.getElapsedTime().asSeconds() > 0.5) {
            m_client.tick();
        }

        // Draw
        m_window.clear();

        m_window.draw(m_player.sprite);
        m_window.draw(m_enemy.sprite);

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