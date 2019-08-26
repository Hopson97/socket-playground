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

namespace {
    struct ClientConnection {
        sf::IpAddress address;
        uint8_t id = -1;
    };

    struct Server {
        sf::UdpSocket socket;
        std::array<ClientConnection, MAX_CLIENTS> clients;
        uint8_t clientCount = 0;
    };

    std::tuple<bool, sf::Packet, sf::IpAddress, uint16_t>
    getIncoming(sf::UdpSocket &socket)
    {
        sf::Packet packet;
        sf::IpAddress senderIp;
        uint16_t senderPort;

        bool success =
            socket.receive(packet, senderIp, senderPort) == sf::Socket::Done;
        return {success, packet, senderIp, senderPort};
    }
} // namespace

void handleConnectionRequest(Server &server, const sf::IpAddress &senderIp,
                             uint16_t senderPort)
{
    sf::Packet packet;
    std::cout << "Connection request recieved\n";
    if (server.clientCount < MAX_CLIENTS) {
        std::cout << "Able to connect\n";

        packet << static_cast<uint8_t>(MessageType::ConnectionAccept);

        ClientConnection client;
        client.address = senderIp;
        client.id = server.clientCount;
        server.clients[server.clientCount] = client;
        server.clientCount++;

        packet << client.id;
        std::cout << "Sending response to " << client.address << "\n";
        if (server.socket.send(packet, senderIp, senderPort) !=
            sf::Socket::Done) {
            std::cout << "Could not connect client :(\n";
            server.clientCount--;
        }
    }
    else {
        packet.clear();
        std::cout << "Cannot be able to connect\n";
        packet << static_cast<uint8_t>(MessageType::ConnectionRefuse);
        packet << "Reason: Server is full.\n";
        if (server.socket.send(packet, senderIp, senderPort) !=
            sf::Socket::Done) {
            std::cout << "Failed to send connection refuse\n";
            server.clientCount--;
        }
    }
}

void runServer()
{
    Server server;
    server.socket.bind(PORT);
    server.socket.setBlocking(false);

    std::atomic<bool> isRunning = true;
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // std::cout << "Waiting for connections\n";
        auto [success, packet, senderIp, senderPort] =
            getIncoming(server.socket);
        if (success) {
            uint8_t type;
            packet >> type;
            switch (static_cast<MessageType>(type)) {
                case MessageType::ConnectionRequest:
                    handleConnectionRequest(server, senderIp, senderPort);
                    break;
                default:
                    break;
            }
        }
    }
}