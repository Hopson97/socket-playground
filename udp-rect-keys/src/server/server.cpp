#include "server.h"

Server::Server(CommandHandler &handler) { m_socket.bind(PORT); }