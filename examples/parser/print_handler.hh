#pragma once

#include "embedded_midi/parser.hh"
#include <print>

struct PrintHandler : EmbeddedMidi::DefaultHandler {
  void handle_undefined_realtime(uint8_t b) {
    std::println("undefine realtime 0x{:x}", b);
  }
  void handle_clock() { std::println("clock"); }
  void handle_start() { std::println("start"); }
  void handle_continue() { std::println("continue"); }
  void handle_stop() { std::println("stop"); }
  void handle_active_sensing() { std::println("active sensing"); }
  void handle_system_reset() { std::println("system reset"); }
  void handle_tune_request() { std::println("tune request"); }

  void handle_sysex_byte(uint8_t b) { std::println("sysex byte {}", b); }

  void handle_end_of_sysex(bool valid) {
    std::println("sysex end, valid: {}", valid);
  }

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
