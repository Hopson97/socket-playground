#include "server.h"

namespace net {
    Server::Server(CommandHandler &handler) 
    { 
        m_socket.bind(PORT); 
        m_commands = std::move(handler);

        m_commands.addCommand(Command::RequestConnection, [this]() {

        })
        .addCommand(Command::Disconnect, [this]() {

        });

        m_commands = std::move(handler);
    }


} // namespace net