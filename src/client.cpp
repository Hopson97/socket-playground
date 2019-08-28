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

namespace {
    struct Client {
        Client(sf::Color c, ClientId id)
            : id(id)
        {
            renderable.setSize({64, 64});
            renderable.setFillColor(c);
            renderable.setOutlineColor(sf::Color::White);
            renderable.setOutlineThickness(2);
        }
        sf::RectangleShape renderable;
        ClientId id;
    };

    void clientSend(sf::UdpSocket &socket, sf::Packet &packet)
    {
        if (socket.send(packet, sf::IpAddress::getLocalAddress(), PORT) !=
            sf::Socket::Done) {
            std::cout << "Could not send.n";
            return;
        }
    }
    
    Client& findClient(std::vector<Client>& clients, ClientId id) {
        for (auto& client : clients) {
            if (client.id == id) {
                return client;
            }
        }

        clients.emplace_back(sf::Color::Red, id);
        return clients.back();
    }

    void run(sf::UdpSocket &socket, const ClientId id)
    {
        sf::RenderWindow window({600, 400}, "Client");
        window.setFramerateLimit(60);
        window.setKeyRepeatEnabled(false);
        socket.setBlocking(false);

        Keyboard keyboard;

        sf::Vector2f pos;

        std::vector<Client> m_playerRenders;
        m_playerRenders.emplace_back(sf::Color::Blue, id);

        while (window.isOpen()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                keyboard.update(e);
                if (e.type == sf::Event::Closed) {
                    window.close();
                }
                else if (e.type == sf::Event::MouseButtonPressed) {
                    auto x = static_cast<float>(e.mouseButton.x);
                    auto y = static_cast<float>(e.mouseButton.y);
                    sf::Packet packet;
                    packet << static_cast<uint8_t>(MessageType::PlayerPosition)
                           << id << x << y;
                    clientSend(socket, packet);
                }
            }
            window.clear();

            // Receive Packets
            auto [success, packet, incomingIp, incomingPort] =
                getIncomingPacket(socket);
            if (success) {
                switch (getMessageType(packet)) {
                    case MessageType::PlayerJoin: {
                        ClientId joinId;
                        packet >> joinId;
                        if (id != joinId) {
                            m_playerRenders.emplace_back(sf::Color::Red, id);
                            std::cout
                                << "New player joined, with ID: " << (int)joinId
                                << '\n';
                        }
                    case MessageType::PlayerPosition:{
                        ClientId moverId;
                        sf::Vector2f position;
                        packet >> moverId >> position.x >> position.y;
                        findClient(m_playerRenders, moverId).renderable.setPosition(position);
                        std::cout << "Got player position, ID: " << int(moverId) << " Player: " << int(id) << " Position: " << position.x << " " << position.y << '\n';
                    }

                    } break;
                    default:
                        break;
                }
            }

            // Render
            for (const auto &rect : m_playerRenders) {
                window.draw(rect.renderable);
            }

            window.display();
        }
    }
} // namespace

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
    ClientId connectionId = -1;
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