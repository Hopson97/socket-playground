#pragma once

#include <SFML/Network.hpp>

namespace net {
    enum class Command : uint16_t {
        // General commands
        RequestConnection,
        AcceptConnection,
        RejectCommand,
        KeepAlive,
        Disconnect,

        // To get number of commands
        CommandCount
    };
}