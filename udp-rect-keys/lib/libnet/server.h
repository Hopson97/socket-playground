#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <array>

#include "event.h"

struct Event;

namespace net {
    /**
     * @brief The server that clients can connect to
     */
    class Server final {
        static constexpr std::size_t MAX_CONNECTIONS = 4;

        /**
         * @brief Each connected client to this server is considered a
         * ConnectedClient
         */
        struct ConnectedClient {
            sf::IpAddress address;
            Port port;
            sf::Time lastUpdate;
        };

      public:
        Server();

        template<typename CommandEnum>
        bool recievePacket(Event &event, sf::Packet &packet, CommandEnum& command)
        {
            if (receiveNetEvent(m_socket, packet, event)) {
                switch (event.type) {
                    case Event::EventType::Connect:
                        handleIncomingConnection(event);
                        break;

                    case Event::EventType::Disconnect:
                        break;

                    case Event::EventType::KeepAlive:
                        keepAlive(event);
                        break;

                    case Event::EventType::DataRecieve:
                        keepAlive(event);
                        packet >> command;
                        break;

                    default:
                        break;
                }
                return true;
            }
            return false;
        }

        bool recievePacket(Event &event, sf::Packet &packet);

      private:
        void handleIncomingConnection(const Event &event);

        void keepAlive(const Event &event);

        std::size_t emptySlot() const;
        ConnectedClient &getClient(ClientId id);

        std::array<ConnectedClient, MAX_CONNECTIONS> m_clients;
        std::array<bool, MAX_CONNECTIONS> m_clientConnected;
        std::size_t currentConnections = 0;

        sf::UdpSocket m_socket;
        sf::Clock m_clock;
    };
} // namespace net