#pragma once

#include <cstdint>

namespace net {
    using Port = std::uint16_t;
    using ClientId = std::uint8_t;

    constexpr Port PORT = 54321;
    constexpr std::size_t CLIENT_COUNT = 4;
} // namespace net