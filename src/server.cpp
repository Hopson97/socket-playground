#include "server.h"
#include "common.h"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>
#include <atomic>
#include <functional>
#include <iostream>
#include <thread>
#include <tuple>

const sf::Time TIMEOUT = sf::seconds(5.0f);

void loghead(const std::string &msg)
{
    std::cout << "\n======\n" << msg << "\n.........\n\n";
}

void log(const std::string &msg) { std::cout << msg << '\n'; }

class Server {
    public:
    struct ClientInformation {
        ClientInformation() = default;
        ClientInformation(sf::IpAddress ipAddress, Port port, ClientId id)
            : ipAddress(ipAddress)
            , port(port)
            , id(id)
            , lastPacketTime(sf::seconds(0))
        {
        }
        sf::IpAddress ipAddress;
        Port port;

        ClientId id;

        sf::Vector2f position;

        sf::Time lastPacketTime;
    };

  public:
    Server();

    void run();

  private:
    void handleIncomingConnectionRequest(const sf::IpAddress &address,
                                         const Port port);
    void handlePlayerPositionUpdate(sf::Packet &packet);

    void handleKeepAlive(sf::Packet& packet);

    ClientInformation& clientFromPacket(sf::Packet& packet);

    void broadcast(sf::Packet &packet, ClientId exclude = 255);

    void clientForeach(std::function<void(ClientId)> func, ClientId exclude = 255);

    int findEmptyClientSlot() const;
    bool isClientConnected(std::size_t slot) const;
    ClientInformation &findById(ClientId id);

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

                case MessageType::KeepAlive:
                    handleKeepAlive(packet);

                default:
                    break;
            }
        }

        clientForeach([this](ClientId id) {
            auto &client = m_clients[static_cast<std::size_t>(id)];
            if ((m_timer.getElapsedTime() - client.lastPacketTime) > TIMEOUT)  {
                std::cout << "A client has disconnected!\n";
                sf::Packet p;
                p << id << static_cast<uint8_t>(MessageType::Disconnect);
                broadcast(p);
                m_clientConnected[id] = false;
            }
        });
    }
}

void Server::broadcast(sf::Packet &packet, ClientId exlude)
{
    clientForeach([this, &packet](ClientId id) {
        auto &client = m_clients[static_cast<std::size_t>(id)];
        m_socket.send(packet, client.ipAddress, client.port);
    }, exlude);
}

void Server::clientForeach(std::function<void(ClientId)> func, ClientId exclude)
{
    for (std::size_t i = 0; i < MAX_CLIENTS; i++) {
        auto clientId= static_cast<ClientId>(i);
        if (isClientConnected(i) && clientId != exclude) {
            func(clientId);
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

        m_clients[freeSlot] = {address, port, static_cast<ClientId>(freeSlot)};
        m_clientConnected[freeSlot] = true;

        sf::Packet packet;
        packet << static_cast<uint8_t>(MessageType::ConnectionAccept)
               << static_cast<ClientId>(freeSlot);
        if (m_socket.send(packet, address, port) != sf::Socket::Done) {
            log("Unable to connect client.\n");
            m_clientConnected[freeSlot] = false;
        }
        log("Connection acceptence sent.");
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
    auto &client = clientFromPacket(packet);
    client.lastPacketTime = m_timer.getElapsedTime();

    sf::Vector2f position;
    packet >> position.x >> position.y;

    client.position = position;

    // Update clients on player positions
    packet.clear();
    packet << static_cast<uint8_t>(MessageType::PlayerPosition) << client.id << position.x << position.y;
    broadcast(packet);
}

void Server::handleKeepAlive(sf::Packet& packet) {
    clientFromPacket(packet).lastPacketTime = m_timer.getElapsedTime();
}

Server::ClientInformation& Server::clientFromPacket(sf::Packet& packet) {
    ClientId id;
    packet >> id;
    return findById(id);
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

Server::ClientInformation& Server::findById(ClientId id)
{
    return m_clients[static_cast<std::size_t>(id)];
}

void runServer() { Server().run(); }