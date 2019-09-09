#include "application.h"

#include <SFML/Window/Event.hpp>

Application::Application()
    : m_client(sf::IpAddress::LocalHost, PORT)
{
}

void Application::run()
{
    m_window.create({1280, 720}, "UDP Socket playground");
    m_window.setFramerateLimit(60);

    while (m_window.isOpen()) {
        pollWindowEvents();

        // Input

        // Update

        // Draw
        m_window.clear();

        m_window.display();
    }
}

void Application::pollWindowEvents()
{
    sf::Event e;
    while (m_window.pollEvent(e)) {
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