#include "command_handler.h"

namespace net {
    CommandHandler &CommandHandler::addCommand(Command command,
                                               std::function<void(void)> f)
    {
        m_commands[static_cast<std::uint16_t>(command)] = f;
    }

    void CommandHandler::invokeCommand(Command command)
    {
        m_commands[static_cast<std::uint16_t>(command)]();
    }
} // namespace net