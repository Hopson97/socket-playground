#include "command_handler.h"

CommandHandler &CommandHandler::addCommand(Command command,
                                           std::function<void(void)> f)
{
    // m_commands[static_cast<std::uint16_t>(command)] = f;
    return *this;
}

void CommandHandler::invokeCommand(Command command)
{
    // m_commands[static_cast<std::uint16_t>(command)]();
}
