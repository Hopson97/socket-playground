#include "client_manager.h"

namespace net {
    ClientManager::ClientManager(const sf::IpAddress &host, Port port)
        : m_hostIp(host)
        , m_hostPort(port)
    {
    }
} // namespace net