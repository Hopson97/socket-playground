#pragma once

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>

#include <array>

#include "../common/common.h"

class Server final {
  public:
    Server();

  private:
    struct ClientConnection {
        ClientId id;
        sf::Time lastUpdate;
    };

    sf::UdpSocket m_socket;
    sf::Clock m_interalClock;
    std::array<ClientConnection, CLIENT_COUNT> m_clients;
};

//#include<functional>
//#include <array>
//#include <cstdio>
//
// template<typename E>
// struct CommandThing {
//    CommandThing() {}
//
//    void give(E command, std::function<void()> f) {
//        cmds[static_cast<std::size_t>(command)] = f;
//    }
//
//    void invoke(E command) {
//        cmds[static_cast<std::size_t>(command)]();
//    }
//
//    std::array<std::function<void()>, static_cast<std::size_t>(E::SIZE)> cmds;
//};
//
// enum class Test {
//    HandleConnect,
//
//    SIZE
//};
//
// int main() {
//    CommandThing<Test> test;
//
//    test.give(Test::HandleConnect, []() {
//        int x = 5;
//        int y =10;
//        printf("%d", x + y);
//    });
//
//    test.invoke(Test::HandleConnect);
//
//}
