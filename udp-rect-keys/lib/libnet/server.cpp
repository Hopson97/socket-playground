#include "server.h"

#include "event.h"

#include <SFML/Network/Packet.hpp>

namespace net {
    Server::Server()
    {
        m_socket.bind(54321);
        m_socket.setBlocking(false);
    }

    bool Server::recievePacket(Event &event)
    {
        sf::Packet packet;
        if (receiveNetEvent(m_socket, packet, event)) {
            switch (event.type) {
                case Event::EventType::Connect:
                    handleIncomingConnection(event);
                    break;

                case Event::EventType::Disconnect:
                    break;

                case Event::EventType::KeepAlive:
                    break;

                case Event::EventType::DataRecieve:
                    break;

                default:
                    break;
            }
            return true;
        }
        return false;
    }

    void Server::handleIncomingConnection(const Event &event)
    {
        if (auto slot = emptySlot(); slot < MAX_CONNECTIONS) {
            m_clientConnected[slot] = true;
            m_clients[slot].address = event.details.senderIp;
            m_clients[slot].port = event.details.senderPort;
            m_clients[slot].lastUpdate = m_clock.getElapsedTime();

            event.respond(m_socket, Event::EventType::AcceptConnection);
        }
        else {
            event.respond(m_socket, Event::EventType::RejectConnection);
        }
    }

    std::size_t Server::emptySlot() const
    {
        for (std::size_t i = 0; i < MAX_CONNECTIONS; i++) {
            if (!m_clientConnected[i]) {
                return i;
            }
        }
        return MAX_CONNECTIONS + 1;
    }

} // namespace net
