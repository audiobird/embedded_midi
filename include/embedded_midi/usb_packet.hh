#pragma once

#include <cstdint>

namespace EmbeddedMidi {

struct USBPacket {
  uint8_t cn_cin{};
  std::array<uint8_t, 3> message;

  USBPacket() = default;

  constexpr uint32_t size() const {
    switch (cn_cin & 0xf) {
    case 0x05:
    case 0x0F: return 1u;
    case 0x02:
    case 0x06:
    case 0x0C:
    case 0x0D: return 2u;
    case 0x03:
    case 0x04:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0E: return 3u;
    default:   return 0u;
    }
  }

  constexpr std::span<const uint8_t> as_raw_stream() const {
    return {message.data(), size()};
  }
};

} // namespace EmbeddedMidi
