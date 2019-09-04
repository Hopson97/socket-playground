#pragma once

#include <array>

#include "commands.h"

namespace net {
    class CommandHandler final {
      public:
        CommandHandler& addCommand(Command command, std::function<void(void)> f);

        void invokeCommand(Command command);
        
      private:
        std::array<std::function<void(void)>, static_cast<std::uint16_t>(Command::CommandCount)> m_commands;
    };
} // namespace net