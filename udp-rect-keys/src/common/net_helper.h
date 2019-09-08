#pragma once

#include <SFML/Network/Packet.hpp>
#include "commands.h"
#include "constants.h"

sf::Packet makePacket(Command command, ClientId clientId);