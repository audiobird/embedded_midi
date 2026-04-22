#pragma once

#include <cstdint>

namespace EmbeddedMidi {

struct USBPacket {
  uint8_t cn_cin{};
  uint8_t message[3];
};

} // namespace EmbeddedMidi
