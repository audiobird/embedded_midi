#include "embedded_midi/parser.hh"
#include "print_handler.hh"

// clang-format off
static constexpr std::array<uint8_t, 256> midi_stream {
    /// various rt messages
    0xf8,
    0xf9,
    0xfa,
    0xfb,
    0xfc,
    0xfd,
    0xfe,
    0xff,

    // note off
    0x80, 32, 13,
    // note off
    0x90, 0xf8, 64, 0xf8, 0, 0xf8,
    // run stat note on
    64, 1,
    // note on new channel
    0x97, 34, 99,
    // note off
    34, 00,

    0xe9,67,67,
    0xd5, 32, 33, 34, 35,

    // sysex with valid 0xf7 end
    0xf0, 1, 2, 0xf8, 3, 4, 5, 6, 7, 0xf7,

    // sysex invalid end 
    0xf0, 10, 20, 0xf8, 30, 40, 0x80,

    // channel mode
    0xbf, 123, 32,

    // MTC quarter frame 
    0xf1, 0x21,
    // INVALID: data byte with no running status
    0x44, 
    // song position pointer 
    0xf2, 0x00, 0x10,
    // song select = 5
    0xf3, 0x05,

    // longer sysex with valid 0xf7 end
    0xf0, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    0xf7,

    // sysex that is too big
    // take note that in the sysex buffer example this overflows
    // but in the non buffered version this is perfectly valid!
    0xf0, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    0xf7,
};
// clang-format on

int main() {
  std::println("Parser example\n");

  {
    EmbeddedMidi::Parser p;
    p.parse(midi_stream, PrintHandler{});
  }

  std::println("\n\nParser example with sysex buffer\n");

  {
    EmbeddedMidi::ParserWithSysexBuffer<32> sp;
    sp.parse(midi_stream, PrintHandler{});
  }
}
