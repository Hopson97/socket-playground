#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <array>
#include <functional>
#include <iostream>

#include "event.h"

struct Event;

namespace net {
    /**
     * @brief The server that clients can connect to
     */
    class Server final {

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
        static constexpr std::size_t MAX_CONNECTIONS = 4;

        Server(OnEventFunction onClientConnect,
               OnEventFunction onClientDisconnect);

        /**
         * @brief Handles stream of all the packets being recieved
         *
         * @tparam CommandEnum
         * @tparam Callback
         * @param callback
         * @return true
         * @return false
         */
        template <typename CommandEnum, typename Callback>
        bool ticking(Callback callback)
        {
            for (std::size_t i = 0 ; i < m_clients.size(); i++) {
                if (m_clientConnected[i]) {
                    if (m_clock.getElapsedTime() - m_clients[i].lastUpdate > sf::seconds(m_timeout)) {
                        auto packet = makePacket(Event::Type::Disconnect, static_cast<ClientId>(i));

                        m_onDisconnect(static_cast<ClientId>(i));
                    }
                }
            }
            Event event;
            sf::Packet packet;
            CommandEnum command;
            while (receiveNetEvent(m_socket, packet, event)) {
                switch (event.type) {
                    case Event::EventType::Connect:
                        handleIncomingConnection(event);
                        break;

                    case Event::EventType::Disconnect:
                        // handle disconnect...
                        m_onDisconnect(event.details);
                        break;

                    case Event::EventType::KeepAlive:
                        keepAlive(event);
                        break;

                    case Event::EventType::Data:
                        keepAlive(event);
                        packet >> command;
                        callback(event.details, packet, command);
                        break;

                    default:
                        break;
                }
                return true;
            }
            return false;
        }

        void sendPacketToPeer(ClientId peerId, sf::Packet &packet);
        void broadcastToPeers(sf::Packet& packet);

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
        sf::Time m_timeout = sf::seconds(3.0);
    };
} // namespace net