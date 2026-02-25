#include "embedded_midi/parser.hh"
#include "embedded_midi/sysex_buffer.hh"
#include <print>

struct PrintParser : EmbeddedMidi::SysexBuffer<32>, EmbeddedMidi::Parser {
  friend class EmbeddedMidi::Parser;

  void handle_clock() { std::println("clock"); }
  void handle_start() { std::println("start"); }
  void handle_continue() { std::println("continue"); }
  void handle_stop() { std::println("stop"); }
  void handle_active_sensing() { std::println("active sensing"); }
  void handle_system_reset() { std::println("system reset"); }
  void handle_tune_request() { std::println("tune request"); }

  void handle_sysex(const std::span<const uint8_t> sysex) {
    std::println("sysex {}", sysex);
  }

  void handle_sysex_overflow(const unsigned amnt) {
    std::println("sysex buffer overflowed by {}", amnt);
  }

  void handle_song_select(const uint8_t song) {
    std::println("song select {}", song);
  }
  void handle_midi_time_code(const uint8_t i) {
    std::println("midi time code {}", i);
  }

  void handle_program_change(const uint8_t chan, const uint8_t pg) {
    std::println("program change: chan {}, prog {}", chan, pg);
  }

  void handle_channel_pressure(const uint8_t chan, const uint8_t v) {
    std::println("channel pressure: chan {}, value {}", chan, v);
  }

  void handle_note_on(const uint8_t chan, const uint8_t note,
                      const uint8_t velocity) {
    std::println("note on: chan {}, note {}, velocity {}", chan, note,
                 velocity);
  }

  void handle_note_off(const uint8_t chan, const uint8_t note,
                       const uint8_t velocity) {
    std::println("note off: chan {}, note {}, velocity {}", chan, note,
                 velocity);
  }

  void handle_control_change(const uint8_t chan, const uint8_t cc,
                             const uint8_t val) {
    std::println("control change: chan {}, cc {}, value {}", chan, cc, val);
  }

  void handle_channel_mode(const uint8_t chan, const uint8_t cc,
                           const uint8_t val) {
    std::println("channel mode: chan {}, cc {}, value {}", chan, cc, val);
  }

  void handle_polyphonic_aftertouch(const uint8_t chan, const uint8_t note,
                                    const uint8_t val) {
    std::println("poly aftertouch: chan {}, note {}, value {}", chan, note,
                 val);
  }

  void handle_pitch_bend(const uint8_t chan, const uint8_t lsb,
                         const uint8_t msb) {
    std::println("pitch bend: chan {}, lsb {}, msb {}", chan, lsb, msb);
  }

  void handle_song_position_pointer(const uint8_t lsb, const uint8_t msb) {
    std::println("song position pointer: lsb {}, msb {}", lsb, msb);
  }
};

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

    // longer sysex with valid 0xf7 end
    0xf0, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8,
    0xf7,

    // sysex that is too big
    0xf0, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    1, 2, 0xf8, 3, 4, 5, 6, 7, 8, 
    0xf7,
};

int main() {
  std::println("Parser example\n");

  PrintParser p;

  p.parse(midi_stream);
}
