#include "embedded_midi/parser.hh"
#include "embedded_midi/usb_packet.hh"
#include "print_handler.hh"
#include <print>

// clang-format off
static constexpr std::array<uint8_t, 256> usb_midi_stream {
    0x0f, 0xf8, 0x00, 0x00,  // timing clock        | cable 0,  CIN F
    0x1f, 0xf9, 0x9c, 0x86,  // rt 0xF9 (undef)     | cable 1,  CIN F, trailing garbage ignored
    0x2f, 0xfa, 0x00, 0x00,  // start               | cable 2,  CIN F
    0x3f, 0xfb, 0x00, 0x00,  // continue            | cable 3,  CIN F
    0x4f, 0xfc, 0xc6, 0xbe,  // stop                | cable 4,  CIN F, garbage
    0x5f, 0xfd, 0x00, 0x00,  // rt 0xFD (undef)     | cable 5,  CIN F
    0x6f, 0xfe, 0x00, 0x00,  // active sensing      | cable 6,  CIN F
    0x7f, 0xff, 0x00, 0x00,  // system reset        | cable 7,  CIN F
    0x82, 0xf1, 0x39, 0xa3,  // MTC quarter frame   | cable 8,  CIN 2 (2-byte), garbage
    0x92, 0xf3, 0x1a, 0x00,  // song select         | cable 9,  CIN 2 (2-byte)
    0xa3, 0xf2, 0x16, 0x6c,  // song position ptr   | cable 10, CIN 3 (3-byte)
    0xb5, 0xf6, 0x00, 0x00,  // tune request        | cable 11, CIN 5 (1-byte sys common)
    0xc4, 0xf0, 0x7d, 0x08,  // sysex start         | cable 12, CIN 4
    0xc4, 0x07, 0x17, 0x37,  // sysex continue      | cable 12, CIN 4
    0xc7, 0x3b, 0x06, 0xf7,  // sysex end (3 bytes) | cable 12, CIN 7
    0xd4, 0xf0, 0x7d, 0x32,  // sysex start         | cable 13, CIN 4
    0xd6, 0x6b, 0xf7, 0x00,  // sysex end (2 bytes) | cable 13, CIN 6
    0xe4, 0xf0, 0x7d, 0x38,  // sysex start         | cable 14, CIN 4
    0xe5, 0xf7, 0x00, 0x00,  // sysex end (1 byte)  | cable 14, CIN 5
    0xf0, 0xf2, 0xc7, 0x81,  // misc/reserved       | cable 15, CIN 0 (ignored)
    0x01, 0xa8, 0xec, 0xd7,  // cable event/reserved| cable 0,  CIN 1 (ignored)
    0x98, 0x80, 0x20, 0x0d,  // note off ch0        | cable 9,  CIN 8
    0xa9, 0x97, 0x22, 0x63,  // note on ch7         | cable 10, CIN 9
    0x3a, 0xa3, 0x47, 0x27,  // poly key pressure   | cable 3,  CIN A, ch3
    0x4b, 0xb0, 0x07, 0x37,  // control change(vol) | cable 4,  CIN B, ch0
    0x5c, 0xc2, 0x56, 0x9a,  // program change      | cable 5,  CIN C (2-byte), ch2, garbage
    0x6d, 0xd9, 0x17, 0x00,  // channel pressure    | cable 6,  CIN D (2-byte), ch9
    0x7e, 0xe4, 0x61, 0x18,  // pitch bend          | cable 7,  CIN E, ch4
    0x58, 0x80, 0x5b, 0x58,  // note off            | cable 5,  CIN 8, ch0
    0x89, 0x95, 0x43, 0x0b,  // note on             | cable 8,  CIN 9, ch5
    0xba, 0xaa, 0x75, 0x1f,  // poly pressure       | cable 11, CIN A, ch10
    0xeb, 0xbf, 0x60, 0x14,  // control change      | cable 14, CIN B, ch15
    0x1c, 0xc4, 0x4b, 0x00,  // program change      | cable 1,  CIN C, ch4
    0x4d, 0xd9, 0x5c, 0x00,  // channel pressure    | cable 4,  CIN D, ch9
    0x7e, 0xee, 0x31, 0x11,  // pitch bend          | cable 7,  CIN E, ch14
    0xa8, 0x83, 0x0b, 0x3a,  // note off            | cable 10, CIN 8, ch3
    0xd9, 0x98, 0x4a, 0x14,  // note on             | cable 13, CIN 9, ch8
    0x0a, 0xad, 0x3b, 0x19,  // poly pressure       | cable 0,  CIN A, ch13
    0x3b, 0xb2, 0x61, 0x47,  // control change      | cable 3,  CIN B, ch2
    0x6c, 0xc7, 0x74, 0x00,  // program change      | cable 6,  CIN C, ch7
    0x9d, 0xdc, 0x5d, 0xa9,  // channel pressure    | cable 9,  CIN D, ch12
    0xce, 0xe1, 0x5e, 0x5a,  // pitch bend          | cable 12, CIN E, ch1
    0xf8, 0x86, 0x35, 0x44,  // note off            | cable 15, CIN 8, ch6
    0x29, 0x9b, 0x12, 0x2b,  // note on             | cable 2,  CIN 9, ch11
    0x5a, 0xa0, 0x3e, 0x29,  // poly pressure       | cable 5,  CIN A, ch0
    0x8b, 0xb5, 0x76, 0x61,  // control change      | cable 8,  CIN B, ch5
    0xbc, 0xca, 0x45, 0xb8,  // program change      | cable 11, CIN C, ch10
    0xed, 0xdf, 0x53, 0x00,  // channel pressure    | cable 14, CIN D, ch15
    0x1e, 0xe4, 0x0e, 0x3a,  // pitch bend          | cable 1,  CIN E, ch4
    0x48, 0x89, 0x08, 0x50,  // note off            | cable 4,  CIN 8, ch9
    0x79, 0x9e, 0x66, 0x44,  // note on             | cable 7,  CIN 9, ch14
    0xaa, 0xa3, 0x10, 0x36,  // poly pressure       | cable 10, CIN A, ch3
    0xdb, 0xb8, 0x50, 0x36,  // control change      | cable 13, CIN B, ch8
    0x0c, 0xcd, 0x7f, 0x00,  // program change      | cable 0,  CIN C, ch13
    0x3d, 0xd2, 0x65, 0x00,  // channel pressure    | cable 3,  CIN D, ch2
    0x6e, 0xe7, 0x75, 0x24,  // pitch bend          | cable 6,  CIN E, ch7
    0x98, 0x8c, 0x43, 0x23,  // note off            | cable 9,  CIN 8, ch12
    0xc9, 0x91, 0x3f, 0x43,  // note on             | cable 12, CIN 9, ch1
    0xfa, 0xa6, 0x6d, 0x66,  // poly pressure       | cable 15, CIN A, ch6
    0x2b, 0xbb, 0x5c, 0x38,  // control change      | cable 2,  CIN B, ch11
    0x5c, 0xc0, 0x23, 0x00,  // program change      | cable 5,  CIN C, ch0
    0x8d, 0xd5, 0x7e, 0x97,  // channel pressure    | cable 8,  CIN D, ch5
    0xbe, 0xea, 0x0c, 0x1c,  // pitch bend          | cable 11, CIN E, ch10
    0xe8, 0x8f, 0x27, 0x28,  // note off            | cable 14, CIN 8, ch15
};
// clang-format on

struct USBPrintHandler : PrintHandler {
  USBPrintHandler(uint32_t vcable) {
    std::print("Virtual Cable 0x{:x}: ", vcable);
  }
};

int main() {
  std::println("USB Parser example\n");

  std::array<EmbeddedMidi::Parser, 16> p{};

  EmbeddedMidi::for_each_usb_packet(
      usb_midi_stream,
      [&p](uint32_t virtual_cable, std::span<const uint8_t> raw_midi) {
        p[virtual_cable].parse(raw_midi, USBPrintHandler{virtual_cable});
      });
}
