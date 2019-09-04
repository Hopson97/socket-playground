#pragma once

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System/Clock.hpp>

namespace net {
    template <typename I> class Server final {
      public:
        Server() { m_socket.bind(PORT); }

      private:
        sf::UdpSocket m_socket;

        sf::Clock m_interalClock;
    };
} // namespace net

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
