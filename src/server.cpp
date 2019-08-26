#include "server.h"
#include "common.h"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include <array>
#include <atomic>
#include <iostream>
#include <thread>

namespace {
    struct ClientConnection {
        sf::IpAddress address;
        uint8_t id = -1;
    };
} // namespace

void runServer()
{
    sf::UdpSocket serverSocket;
    serverSocket.bind(PORT);
    serverSocket.setBlocking(false);
    std::array<ClientConnection, MAX_CLIENTS> clients;
    uint8_t clientCount = 0;

    std::atomic<bool> isRunning = true;

    // clientListener.setBlocking(false);

    sf::Packet packet;
    sf::IpAddress senderIpAddress;
    unsigned short senderPort;
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // std::cout << "Waiting for connections\n";
        if (serverSocket.receive(packet, senderIpAddress, senderPort) ==
            sf::Socket::Done) {
            uint8_t type;
            packet >> type;

            switch (static_cast<MessageType>(type)) {

                case MessageType::ConnectionRequest:
                    packet.clear();
                    std::cout << "Connection request recieved\n";
                    if (clientCount < MAX_CLIENTS) {
                        std::cout << "Able to connect\n";

                        packet << static_cast<uint8_t>(
                            MessageType::ConnectionAccept);

                        ClientConnection client;
                        client.address = senderIpAddress;
                        client.id = clientCount;
                        clients[clientCount] = client;
                        clientCount++;

                        packet << client.id;
                        std::cout << "Sending response to " << client.address
                                  << "\n";
                        if (serverSocket.send(packet, senderIpAddress,
                                              senderPort) != sf::Socket::Done) {
                            std::cout << "Could not connect client :(\n";
                            clientCount--;
                        }
                    }
                    else {
                        packet.clear();
                        std::cout << "Cannot be able to connect\n";
                        packet << static_cast<uint8_t>(
                            MessageType::ConnectionRefuse);
                        packet << "Reason: Server is full.\n";
                        if (serverSocket.send(packet, senderIpAddress,
                                              senderPort) != sf::Socket::Done) {
                            std::cout << "Failed to send connection refuse\n";
                            clientCount--;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}