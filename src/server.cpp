#include "server.h"
#include "common.h"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>
#include <atomic>
#include <iostream>
#include <thread>
#include <tuple>

void loghead(const std::string &msg)
{
    std::cout << "\n======\n" << msg << "\n.........\n\n";
}

void log(const std::string &msg) { std::cout << msg << '\n'; }

class Server {
    struct ClientInformation {
        ClientInformation() = default;
        ClientInformation(sf::IpAddress ipAddress, Port port, uint8_t id)
            : ipAddress(ipAddress)
            , port(port)
            , id(id)
            , timeSinceLastPacket(sf::seconds(0))
        {
        }
        sf::IpAddress ipAddress;
        Port port;

        uint8_t id;

        sf::Vector2f position;

        sf::Time timeSinceLastPacket;
    };

  public:
    Server();

    void run();

  private:
    void handleIncomingConnectionRequest(const sf::IpAddress &address,
                                         const Port port);
    void handlePlayerPositionUpdate(sf::Packet &packet);
    void broadcast(sf::Packet &packet);

    int findEmptyClientSlot() const;
    bool isClientConnected(std::size_t slot) const;
    ClientInformation &findById(uint8_t id);

    sf::UdpSocket m_socket;
    std::array<ClientInformation, MAX_CLIENTS> m_clients;
    std::array<bool, MAX_CLIENTS> m_clientConnected;

    std::atomic<bool> m_isServerRunning;

    sf::Clock m_timer;
};

Server::Server()
{
    m_clientConnected.fill(false);
    m_socket.bind(PORT);
    m_socket.setBlocking(false);
}

void Server::run()
{
    while (m_isServerRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto [success, packet, incomingIp, incomingPort] =
            getIncomingPacket(m_socket);
        if (success) {
            switch (getMessageType(packet)) {
                case MessageType::ConnectionRequest:
                    handleIncomingConnectionRequest(incomingIp, incomingPort);
                    break;

                case MessageType::PlayerPosition:
                    handlePlayerPositionUpdate(packet);
                    break;

                default:
                    break;
            }
        }
    }
}

void Server::broadcast(sf::Packet &packet)
{
    for (std::size_t i = 0; i < MAX_CLIENTS; i++) {
        if (isClientConnected(i)) {
            auto &client = m_clients[i];

            m_socket.send(packet, client.ipAddress, client.port);
        }
    }
}

void Server::handleIncomingConnectionRequest(const sf::IpAddress &address,
                                             const Port port)
{
    loghead("Incoming connection requested.");
    int freeSlot = findEmptyClientSlot();
    if (freeSlot != -1) {
        log("Connection is able to be established\n");

        m_clients[freeSlot] = {address, port, static_cast<uint8_t>(freeSlot)};
        m_clientConnected[freeSlot] = true;

        sf::Packet packet;
        packet << static_cast<uint8_t>(MessageType::ConnectionAccept)
               << static_cast<uint8_t>(freeSlot);
        if (m_socket.send(packet, address, port) != sf::Socket::Done) {
            log("Unable to connect client.\n");
            m_clientConnected[freeSlot] = false;
        }
        log("Connection acceptence sent.");

        packet.clear();
        packet << static_cast<uint8_t>(MessageType::PlayerJoin) << static_cast<uint8_t>(freeSlot);
        broadcast(packet);
    }
    else {
        log("Connection is not able to be established as server is full.\n");
        sf::Packet packet;
        packet << static_cast<uint8_t>(MessageType::ConnectionRefuse)
               << "Server is full.";
        if (m_socket.send(packet, address, port) != sf::Socket::Done) {
            log("Unable to send client connection refusal.\n");
        }
    }
}

void Server::handlePlayerPositionUpdate(sf::Packet &packet)
{
    loghead("Incoming: Position");
    uint8_t id;
    packet >> id;
    auto &client = findById(id);
    client.timeSinceLastPacket = sf::Time::Zero;

    sf::Vector2f position;
    packet >> position.x >> position.y;

    client.position = position;

    sf::Packet send;
    send << static_cast<uint8_t>(MessageType::PlayerPosition) << id
         << position.x << position.y;
}

int Server::findEmptyClientSlot() const
{
    for (std::size_t i = 0; i < MAX_CLIENTS; i++) {
        if (!isClientConnected(i)) {
            return i;
        }
    }
    return -1;
}

bool Server::isClientConnected(std::size_t slot) const
{
    return m_clientConnected[slot];
}

Server::ClientInformation &Server::findById(uint8_t id)
{
    return m_clients[static_cast<std::size_t>(id)];
}

void runServer() { Server().run(); }