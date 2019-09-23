#include "server.h"

#include "event.h"

#include <SFML/Network/Packet.hpp>
#include <iostream>

namespace net {
    Server::Server(OnEventFunction onClientConnect,
                   OnEventFunction onClientDisconnect)
        : m_onConnect(onClientConnect)
        , m_onDisconnect(onClientDisconnect)
    {
        m_socket.bind(54321);
        m_socket.setBlocking(false);
        m_clientConnected.fill(false);
    }

    void Server::sendPacketToPeer(ClientId peerId, sf::Packet &packet)
    {
        if (m_clientConnected[peerId]) {
            auto &client = getClient(peerId);
            m_socket.send(packet, client.address, client.port);
        }
    }

    void Server::broadcastToPeers(sf::Packet& packet) {
        for (std::size_t i = 0 ; i < m_clients.size(); i++) {
            sendPacketToPeer(static_cast<ClientId>(i), packet);
        }
    }

    void Server::handleIncomingConnection(const Event &event)
    {
        if (auto slot = emptySlot(); slot < MAX_CONNECTIONS) {
            m_clientConnected[slot] = true;
            m_clients[slot].address = event.details.senderIp;
            m_clients[slot].port = event.details.senderPort;
            m_clients[slot].lastUpdate = m_clock.getElapsedTime();

            event.respond(m_socket, Event::EventType::AcceptConnection,
                          static_cast<ClientId>(slot));
            
            m_onConnect({event.details.senderIp, event.details.senderPort, static_cast<ClientId>(slot)});
        }
        else {
            event.respond(m_socket, Event::EventType::RejectConnection);
        }
    }

    void Server::keepAlive(const Event &event)
    {
        auto &client = getClient(event.details.id);
        client.lastUpdate = m_clock.getElapsedTime();
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

    Server::ConnectedClient &Server::getClient(ClientId id)
    {
        return m_clients[static_cast<std::size_t>(id)];
    }

} // namespace net
