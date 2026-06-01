#pragma once

#include <cassert>
#include <cstdint>
#include <span>

namespace EmbeddedMidi {

inline constexpr void for_each_usb_packet(std::span<const uint8_t> stream,
                                          auto &&handler) {
  assert(!(stream.size() % 4));

  for (auto i = 0u; i < stream.size(); i += 4) {
    switch (stream[i] & 0xf) {
    case 0x00:
    case 0x01:
      break;
    case 0x05:
    case 0x0f:
      handler(stream[i] >> 4, std::span<const uint8_t>{&stream[i + 1], 1});
      break;
    case 0x02:
    case 0x06:
    case 0x0C:
    case 0x0D:
      handler(stream[i] >> 4, std::span<const uint8_t>{&stream[i + 1], 2});
      break;
    default:
      handler(stream[i] >> 4, std::span<const uint8_t>{&stream[i + 1], 3});
      break;
    }
  }
}

} // namespace EmbeddedMidi
