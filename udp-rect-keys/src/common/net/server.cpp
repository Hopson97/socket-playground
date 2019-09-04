#include "server.h"

namespace net {
    Server::Server() { m_socket.bind(PORT); }
} // namespace net