#pragma once

#include <array>
#include <functional>

#include "commands.h"

class CommandHandler final {
    using Func = std::function<void()>;

  public:
    CommandHandler &addCommand(Command command, Func f);

    void invokeCommand(Command command);

  private:
    std::array<Func, static_cast<std::uint16_t>(Command::CommandCount)>
        m_commands;
};
