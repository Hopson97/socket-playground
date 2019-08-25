#include <iostream>

#include <SFML/Network.hpp>
#include <array>
#include <atomic>
#include <mutex>
#include <thread>

constexpr uint16_t PORT = 43211;
constexpr uint8_t MAX_CLIENTS = 4;

/**
 * The message object which is sent between clients and the server
 */
struct Message {
    enum class Type {
        ClientMessage,
        ServerStop,
    };

    Type type;
    std::string message;
    std::string name;
};

/**
 * @brief Create an sf::Packet ready for sending using the message
 *
 * @param packet The packet to send
 * @param message The message to send
 * @return sf::Packet& The packet to send
 */
sf::Packet &operator<<(sf::Packet &packet, const Message &message)
{
    return packet << static_cast<uint8_t>(message.type) << message.name
                  << message.message;
}

/**
 * @brief Create an sf::Packet ready for recieving using the message
 *
 * @param packet The packet to recieve
 * @param message The message to recieve
 * @return sf::Packet& The packet to recieve
 */
sf::Packet &operator>>(sf::Packet &packet, Message &message)
{
    uint8_t type = 0;
    packet >> type >> message.name >> message.message;
    message.type = static_cast<Message::Type>(type);
    return packet;
}

/**
 * @brief Runs the server
 *
 */
void runServer()
{
    sf::TcpListener clientListener;
    std::array<sf::TcpSocket, MAX_CLIENTS> clients;
    uint8_t clientCount = 0;

    std::mutex mu;
    std::atomic<bool> isRunning = true;

    clientListener.setBlocking(false);

    if (clientListener.listen(PORT) != sf::Socket::Done) {
        std::cout << "Unable to listen on port " << PORT << "\n";
    }

    // For handling exiting the server thing
    std::thread consoleThread([&isRunning, &mu, &clientCount]() {
        char input;
        std::cout << "Enter to exit\n";
        std::cin >> input;
        isRunning = false;
    });

    // Listens for messages from the client, and broadcasts
    std::thread clientInteractThread([&isRunning, &clientCount, &clients,
                                      &clientListener, &mu]() {
        sf::Packet recievedPacket;
        while (isRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            for (int i = 0; i < clientCount; i++) {
                std::unique_lock<std::mutex> lock(mu);
                if (clients[i].receive(recievedPacket) == sf::Socket::Done) {
                    std::cout << "Got a message!";
                    Message msg;
                    recievedPacket >> msg;
                    for (int i = 0; i < clientCount; i++) {
                        sf::Packet packet;
                        packet << msg;
                        if (clients[i].send(packet) != sf::Socket::Done) {
                            std::cout << "Failed to broadcast message\n";
                        }
                    }
                }
            }
        }
        std::unique_lock<std::mutex> lock(mu);
        sf::Packet packet;
        Message msg;
        msg.type = Message::Type::ServerStop;
        packet << msg;
        for (int i = 0; i < clientCount; i++) {
            clients[i].send(packet);
        }
    });

    // Listens for new connections
    std::thread clientConnectionsThread(
        [&isRunning, &clientCount, &clients, &clientListener, &mu]() {
            while (isRunning) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                if (clientCount < MAX_CLIENTS) {
                    auto &client = clients[clientCount];
                    if (clientListener.accept(client) == sf::Socket::Done) {

                        std::unique_lock<std::mutex> lock(mu);
                        std::cout << "New client connected!\n";
                        client.setBlocking(false);
                        clientCount++;
                    }
                }
            }
        });

    consoleThread.join();
    clientConnectionsThread.join();
    clientInteractThread.join();
}

/**
 * @brief Runs the client
 *
 * @param name The name of the user of the client
 */
void runClient(const std::string &name)
{
    sf::TcpSocket socket;
    if (socket.connect(sf::IpAddress::LocalHost, PORT) != sf::Socket::Done) {
        std::cout << "Failed to connect to server.\n";
        return;
    }
    std::cout << "Connected to server\n";
    auto sendMessage = "Enter something to send:\n ";

    std::mutex mu;
    socket.setBlocking(false);

    std::atomic<bool> isRunning = true;

    // Listens for messages from other clients
    std::thread listener([&mu, &socket, &isRunning] {
        while (isRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            sf::Packet packet;
            Message message;
            std::unique_lock<std::mutex> lock(mu);
            if (socket.receive(packet) == sf::Socket::Done) {
                packet >> message;
                switch (message.type) {
                case Message::Type::ClientMessage:
                    std::cout << message.name << ": " << message.message
                              << '\n';
                    break;

                case Message::Type::ServerStop:
                    isRunning = false;
                    return;

                default:
                    break;
                }
            }
        }
    });

    // For sending messages to other clients
    std::thread sender([&mu, &socket, &isRunning, sendMessage, name] {
        while (isRunning) {
            sf::Packet packet;
            Message message;
            std::cout << sendMessage;
            std::getline(std::cin, message.message);
            message.name = name;
            message.type = Message::Type::ClientMessage;
            packet << message;

            std::unique_lock<std::mutex> lock(mu);
            if (socket.send(packet) != sf::Socket::Done) {
                std::cout << "Failed to send message\n";
            }
        }
    });

    sender.join();
    listener.join();
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