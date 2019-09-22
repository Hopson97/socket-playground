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

                    case Event::EventType::RejectConnection:

                    default:
                        break;
                }
            }
        }
    }

    Client::~Client() 
    {

    }

    bool Client::send(sf::Packet &packet)
    {
        return m_socket.send(packet, m_serverAddress, m_serverPort) ==
               sf::Socket::Done;
    }

    bool Client::receive(sf::Packet &packet)
    {
        return m_socket.receive(packet, m_recievedIp, m_recievedPort) ==
               sf::Socket::Done;
    }

    void Client::handlePeerConnection(const Event &event)
    {

        m_onPeerConnect(event.details);
    }
} // namespace net
