#include "server.h"
#include "common.h"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include <array>
#include <atomic>
#include <iostream>
#include <thread>
#include <tuple>

void loghead(const std::string &msg)
{
    std::cout << "\n===\n==\n" << msg << "\n-";
}

void log(const std::string &msg) 
{ 
    std::cout << "\n---\n" << msg << "\n-"; 
}

class Server {
    struct ClientInformation {
        ClientInformation() = default;
        ClientInformation(sf::IpAddress ipAddress, Port port, uint8_t id)
            : ipAddress(ipAddress)
            , port(port)
            , id(id)
        {
        }
        sf::IpAddress ipAddress;
        Port port;

        uint8_t id;

        sf::Time timeOutTimer;
    };

  public:
    Server();

    void run();

  private:
    std::tuple<bool, sf::Packet, sf::IpAddress, Port> getIncomingPacket();
    void handleIncomingConnectionRequest(const sf::IpAddress &address,
                                         const Port port);

    int findEmptyClientSlot() const;
    bool isClientConnected(std::size_t slot) const;

    sf::UdpSocket m_socket;
    std::array<ClientInformation, MAX_CLIENTS> m_clients;
    std::array<bool, MAX_CLIENTS> m_clientConnected;

    std::atomic<bool> m_isServerRunning;
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
        auto [success, packet, incomingIp, incomingPort] = getIncomingPacket();
        if (success) {
            switch (getMessageType(packet)) {
                case MessageType::ConnectionRequest:
                    handleIncomingConnectionRequest(incomingIp, incomingPort);
                    break;

                default:
                    break;
            }
        }
    }
}

std::tuple<bool, sf::Packet, sf::IpAddress, Port> Server::getIncomingPacket()
{
    sf::Packet packet;
    sf::IpAddress incomingIp;
    Port incomingPort;
    bool gotPacket =
        m_socket.receive(packet, incomingIp, incomingPort) == sf::Socket::Done;
    return {gotPacket, packet, incomingIp, incomingPort};
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
               << freeSlot;
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

int Server::findEmptyClientSlot() const
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
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

void runServer() { Server().run(); }