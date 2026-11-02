#pragma once

#include "midi_packet.hh"
#include <cassert>
#include <cstdint>
#include <span>

namespace EmbeddedMidi {

struct USBPacket {
  uint8_t cn_cin{};
  std::array<uint8_t, 3> message;

  USBPacket() = default;

  USBPacket(uint8_t cable, Raw1ByteMessage m) {
    *this = from_message(cable, m);
  }

  USBPacket(uint8_t cable, Raw2ByteMessage m) {
    *this = from_message(cable, m);
  }

  USBPacket(uint8_t cable, Raw3ByteMessage m) {
    *this = from_message(cable, m);
  }

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

  static constexpr USBPacket from_message(uint8_t cable, Raw1ByteMessage m) {
    USBPacket out;
    out.cn_cin = encoder_cn_cin(cable, m[0] >= 0xf8 ? 0xf : 0x5);
    out.message[0] = m[0];
    return out;
  }

  static constexpr USBPacket from_message(uint8_t cable, Raw2ByteMessage m) {
    USBPacket out;
    if (m[0] >= 0xc) {
      out.cn_cin = encoder_cn_cin(cable, m[0] >> 4);
    } else if (m[1] == 0xf7) {
      out.cn_cin = encoder_cn_cin(cable, 0x6);
    } else {
      out.cn_cin = encoder_cn_cin(cable, 0x2);
    }
    out.message[0] = m[0];
    out.message[1] = m[1];
    return out;
  }

  static constexpr USBPacket from_message(uint8_t cable, Raw3ByteMessage m) {
    USBPacket out;
    if (m[0] >= 0x8) {
      out.cn_cin = encoder_cn_cin(cable, m[0] >> 4);
    } else if (m[2] == 0xf7) {
      out.cn_cin = encoder_cn_cin(cable, 0x7);
    } else if (m[0] == 0xf0 || m[0] < 0x80) {
      out.cn_cin = encoder_cn_cin(cable, 0x4);
    } else {
      out.cn_cin = encoder_cn_cin(cable, 0x3);
    }
    out.message[0] = m[0];
    out.message[1] = m[1];
    out.message[2] = m[2];
    return out;
  }

private:
  static constexpr uint8_t encoder_cn_cin(uint8_t cable, uint8_t cin) {
    return (cable << 4) | cin;
  }
};

} // namespace EmbeddedMidi
