#pragma once

#include <cstdint>

constexpr uint16_t PORT = 42124;
constexpr uint16_t PORT1 = 42125;
constexpr uint8_t MAX_CLIENTS = 4;

enum class MessageType {
    ConnectionRequest,
    ConnectionAccept,
    ConnectionRefuse,
};