#pragma once

#include <array>

namespace net {
    template <typename I> class CommandHandler final {
      public:
      private:
        std::array<I, I::COUNT> m_commands;
    };
} // namespace net