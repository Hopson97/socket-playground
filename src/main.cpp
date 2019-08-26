#include <iostream>

#include "input/keyboard.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <array>
#include <atomic>
#include <mutex>
#include <thread>

constexpr uint16_t PORT = 42124;
constexpr uint16_t PORT1 = 42125;
constexpr uint8_t MAX_CLIENTS = 4;

enum class MessageType {
    ConnectionRequest,
    ConnectionAccept,
    ConnectionRefuse,
};

struct ClientConnection {
    sf::IpAddress address;
    uint8_t id = -1;
};

void runServer()
{
    sf::UdpSocket serverSocket;
    serverSocket.bind(PORT);
    serverSocket.setBlocking(false);
    std::array<ClientConnection, MAX_CLIENTS> clients;
    uint8_t clientCount = 0;

    std::mutex mu;
    std::atomic<bool> isRunning = true;

    // clientListener.setBlocking(false);

    sf::Packet packet;
    sf::IpAddress senderIpAddress;
    unsigned short senderPort;
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //std::cout << "Waiting for connections\n";
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
        break;

    case MessageType::ConnectionRefuse:{
        std::cout << "Refused\n";
        std::string reason;
        packet >> reason;
        std::cout << "Reason: " << reason << std::endl;
        break;}

    default:
        break;
    }
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cout << "Please enter `server/client name`" << std::endl;
        return 0;
    }

    std::string kind = argv[1];
    std::string name = argv[2];

    if (kind == "server") {
        runServer();
    }
    else if (kind == "client") {
        runClient(name);
    }
}