#include "client.h"
#include "common.h"
#include "input/keyboard.h"

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include <SFML/Graphics.hpp>

#include <array>
#include <atomic>
#include <iostream>
#include <thread>

namespace
{
    void clientSend(sf::UdpSocket& socket, sf::Packet& packet) {
        if (socket.send(packet, sf::IpAddress::getLocalAddress(), PORT) !=
                sf::Socket::Done) {
                std::cout << "Could not send.n";
                return;
            }
    }

    void run([[maybe_unused]]sf::UdpSocket& socket, [[maybe_unused]]const uint8_t id) {
        sf::RenderWindow window({600, 400}, "Client");
        window.setFramerateLimit(60);
        window.setKeyRepeatEnabled(false);

        Keyboard keyboard;

        sf::Vector2f pos;

        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                keyboard.update(e);
                if (e.type == sf::Event::Closed ) {
                    window.close();
                }
            }
            window.clear();
            sf::Packet packet;
            packet << static_cast<int>(MessageType::PlayerPosition) << id << pos.x << pos.y;
            clientSend(socket, packet);
            window.display();
        }
    }
}

void runClient(std::string name)
{
    sf::UdpSocket clientSocket;
    sf::Packet packet;
    packet << static_cast<uint8_t>(MessageType::ConnectionRequest)
           << sf::IpAddress::getLocalAddress().toString() << name;

    if (clientSocket.send(packet, sf::IpAddress::getLocalAddress(), PORT) !=
        sf::Socket::Done) {
        std::cout << "Could not connect.n";
        return;
    }
    std::cout << "Connection request sent.\n";

    sf::IpAddress senderIpAddress;
    unsigned short senderPort;
    packet.clear();
    if (clientSocket.receive(packet, senderIpAddress, senderPort) !=
        sf::Socket::Done) {
        std::cout << "Connection refused: Unable to recieve data.\n";
        return;
    }
    std::cout << "Response recieved!\nSize: " << packet.getDataSize() << '\n';
    uint8_t type;
    packet >> type;
    std::cout << "Connection type: " << (int)type << std::endl;
    uint8_t connectionId = -1;
    switch (static_cast<MessageType>(type)) {
        case MessageType::ConnectionAccept:
            packet >> connectionId;
            std::cout << "Connected with ID: " << (int)connectionId << '\n';
            run(clientSocket, connectionId);
            break;

        case MessageType::ConnectionRefuse: {
            std::cout << "Refused\n";
            std::string reason;
            packet >> reason;
            std::cout << "Reason: " << reason << std::endl;
            break;
        }

        default:
            break;
    }
}