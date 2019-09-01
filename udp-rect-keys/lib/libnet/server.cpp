#include "server.h"

#include <iostream>

#include "common.h"

namespace net {
    Server::Server() {
        m_socket.bind(PORT);
    }
}