#pragma once

#include <array>

#include "commands.h"

namespace net {
    class CommandHandler final {
      public:
      private:
        std::array<Command, static_cast<std::size_t>(Command::CommandCount)>
            m_commands;
    };
} // namespace net