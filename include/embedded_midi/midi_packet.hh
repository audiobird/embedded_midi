#pragma once

#include <array>
#include <cstdint>

namespace EmbeddedMidi {

using Raw1ByteMessage = std::array<uint8_t, 1>;
using Raw2ByteMessage = std::array<uint8_t, 2>;
using Raw3ByteMessage = std::array<uint8_t, 3>;

class Message {
public:
  static constexpr Raw3ByteMessage note_on(uint8_t channel, uint8_t note,
                                           uint8_t velocity) {
    return channel_message_3(0x90, channel, note, velocity);
  }

  static constexpr Raw3ByteMessage note_off(uint8_t channel, uint8_t note,
                                            uint8_t velocity = 0) {
    return channel_message_3(0x80, channel, note, velocity);
  }

  static constexpr Raw3ByteMessage
  polyphonic_aftertouch(uint8_t channel, uint8_t note, uint8_t value) {
    return channel_message_3(0xa0, channel, note, value);
  }

  static constexpr Raw3ByteMessage
  control_change(uint8_t channel, uint8_t controller, uint8_t value) {
    return channel_message_3(0xb0, channel, controller, value);
  }

  static constexpr Raw3ByteMessage
  channel_mode(uint8_t channel, uint8_t controller, uint8_t value) {
    return control_change(channel, controller, value);
  }

  static constexpr Raw3ByteMessage pitch_bend(uint8_t channel, uint8_t lsb,
                                              uint8_t msb) {
    return channel_message_3(0xe0, channel, lsb, msb);
  }

  static constexpr Raw2ByteMessage program_change(uint8_t channel, uint8_t pc) {
    return channel_message_2(0xc0, channel, pc);
  }

  static constexpr Raw2ByteMessage channel_pressure(uint8_t channel,
                                                    uint8_t value) {
    return channel_message_2(0xd0, channel, value);
  }

  static constexpr Raw1ByteMessage clock() { return {0xf8}; }
  static constexpr Raw1ByteMessage start() { return {0xfa}; }

  // name clashes with continue keyword, hence the suffix
  static constexpr Raw1ByteMessage continue_rt() { return {0xfb}; }
  static constexpr Raw1ByteMessage stop() { return {0xfc}; }
  static constexpr Raw1ByteMessage active_sensing() { return {0xfe}; }
  static constexpr Raw1ByteMessage system_reset() { return {0xff}; }

private:
  static constexpr Raw2ByteMessage
  channel_message_2(uint8_t nibble, uint8_t channel, uint8_t d0) {
    return {static_cast<uint8_t>(nibble | channel), d0};
  }

  static constexpr Raw3ByteMessage
  channel_message_3(uint8_t nibble, uint8_t channel, uint8_t d0, uint8_t d1) {
    return {static_cast<uint8_t>(nibble | channel), d0, d1};
  }
};

} // namespace EmbeddedMidi
