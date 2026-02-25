#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace EmbeddedMidi {

template <unsigned size_> class SysexBuffer {
  friend class Parser;

  std::array<uint8_t, size_> buf{};
  uint32_t cnt{};

  constexpr void handle_sysex_byte(const uint8_t data) {
    if (cnt < size_) {
      buf[cnt++] = data;
      return;
    }

    ++cnt;
  }

  constexpr void handle_end_of_sysex(this auto &self, const bool valid) {
    auto &t = static_cast<SysexBuffer &>(self);

    if (valid) {
      if (t.cnt > size_) {
        self.handle_sysex_overflow(t.cnt - size_);
      } else {
        self.handle_sysex(std::span<const uint8_t>{t.buf.data(), t.cnt});
      }
    }

    t.cnt = {};
  }

  constexpr void handle_sysex_overflow(const uint32_t overflow_amnt) {}
};

} // namespace EmbeddedMidi
