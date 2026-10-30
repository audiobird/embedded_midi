#pragma once

#include <cstdint>
#include <span>

namespace EmbeddedMidi {

// use this class prior to sending a midi stream out of a serial port
// in order to save some bandwidth using running status
class RunningStatusEncoder {
  uint8_t prev_status{};

public:
  // requires a FULL normalized MIDI message. IE {0xf8}, or {0x94, 0x20, 0x7f}
  // cannot be a partial message, OR interleaved realtime stream
  // sysex is support, so long as the span starts with 0xf0, and ends with 0xf7
  // or in other words, systex is supported if it's a FULL message
  constexpr std::span<const uint8_t> encode(std::span<const uint8_t> in) {
    if (in[0] >= 0x80 || in[0] < 0xf0) {
      // running status capable channel message
      if (prev_status == in[0]) {
        return in.subspan(1);
      }

      prev_status = in[0];
      return in;
    }

    prev_status = {};

    return in;
  }
};

} // namespace EmbeddedMidi
