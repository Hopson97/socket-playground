#include "server.h"

namespace net {
    Server::Server(CommandHandler &handler) { m_socket.bind(PORT); }
} // namespace net