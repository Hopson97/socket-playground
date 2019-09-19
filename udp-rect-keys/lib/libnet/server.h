#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <functional>
#include <array>

#include "event.h"

struct Event;

namespace net {
    /**
     * @brief The server that clients can connect to
     */
    class Server final {
        static constexpr std::size_t MAX_CONNECTIONS = 4;

        using OnEventFunction = std::function<void(const Event::Details& details)>;

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

        void onClientConnect(OnEventFunction function);
        void onClientDisconnect(OnEventFunction function);

        template <typename CommandEnum>
        bool recievePacket(Event::Details &details, sf::Packet &packet,
                           CommandEnum &command)
        {
            Event event;
            if (receiveNetEvent(m_socket, packet, event)) {
                switch (event.type) {
                    case Event::EventType::Connect:
                        handleIncomingConnection(event);
                        break;

                    case Event::EventType::Disconnect:
                        //handle disconnect...
                        m_onDisconnect(event.details);
                        break;

                    case Event::EventType::KeepAlive:
                        keepAlive(event);
                        break;

                    case Event::EventType::DataRecieve:
                        keepAlive(event);
                        packet >> command;
                        details = event.details;
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

        OnEventFunction m_onConnect;
        OnEventFunction m_onDisconnect;

        std::size_t emptySlot() const;
        ConnectedClient &getClient(ClientId id);

        std::array<ConnectedClient, MAX_CONNECTIONS> m_clients;
        std::array<bool, MAX_CONNECTIONS> m_clientConnected;
        std::size_t currentConnections = 0;

        sf::UdpSocket m_socket;
        sf::Clock m_clock;
    };
} // namespace net