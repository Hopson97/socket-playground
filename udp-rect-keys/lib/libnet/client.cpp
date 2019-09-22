#include "client.h"

#include <iostream>

#include "event.h"
#include "packet_factory.h"

#include <SFML/Network/Packet.hpp>

namespace net {
    Client::Client(const sf::IpAddress &serverAddress, Port serverPort,
                   OnEventFunction onPeerConnect,
                   OnEventFunction onPeerDisconnect)
        : m_serverAddress(serverAddress)
        , m_serverPort(serverPort)
        , m_onPeerConnect(onPeerConnect)
        , m_onPeerDisconnect(onPeerDisconnect)
    {
        auto packet = makePacket(Event::EventType::Connect);
        if (send(packet)) {
            packet.clear();
            Event event;
            if (receiveNetEvent(m_socket, packet, event)) {
                switch (event.type) {
                    case Event::EventType::AcceptConnection:
                        m_id = event.details.id;
                        m_socket.setBlocking(false);
                        m_isConnected = true;
                        break;

                    case Event::EventType::RejectConnection:
                        break;

                    default:
                        break;
                }
            }
        }
    }

    Client::~Client() {}

    bool Client::send(sf::Packet &packet)
    {
        return m_socket.send(packet, m_serverAddress, m_serverPort) ==
               sf::Socket::Done;
    }

    void Client::handlePeerConnection(const Event &event)
    {

        m_onPeerConnect(event.details);
    }

    bool Client::connected() const {
        return m_isConnected;
    }

    ClientId Client::getClientId() const { return m_id; }
} // namespace net
