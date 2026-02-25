#include "embedded_midi/parser.hh"
#include "embedded_midi/sysex_buffer.hh"

// starting point handler struct
struct BareHandler : EmbeddedMidi::Parser {
  void handle_clock() {}
  void handle_start() {}
  void handle_continue() {}
  void handle_stop() {}
  void handle_active_sensing() {}
  void handle_system_reset() {}
  void handle_tune_request() {}

  void handle_sysex_byte(const uint8_t data) {}
  void handle_end_of_sysex(const bool valid) {}

  void handle_song_select(const uint8_t song) {}

  void handle_midi_time_code(const uint8_t i) {}

  void handle_program_change(const uint8_t chan, const uint8_t pg) {}

  void handle_channel_pressure(const uint8_t chan, const uint8_t v) {}

  void handle_note_on(const uint8_t chan, const uint8_t note,
                      const uint8_t velocity) {}

  void handle_note_off(const uint8_t chan, const uint8_t note,
                       const uint8_t velocity) {}

  void handle_channel_mode(const uint8_t chan, const uint8_t cc,
                           const uint8_t val) {}

  void handle_control_change(const uint8_t chan, const uint8_t cc,
                             const uint8_t val) {}

  void handle_polyphonic_aftertouch(const uint8_t chan, const uint8_t note,
                                    const uint8_t val) {}

  void handle_pitch_bend(const uint8_t chan, const uint8_t lsb,
                         const uint8_t msb) {}

  void handle_song_position_pointer(const uint8_t lsb, const uint8_t msb) {}
};

// starting point handler struct with a sysex buffer
struct BareHandlerWithSysexBuffer : EmbeddedMidi::Parser,
                                    EmbeddedMidi::SysexBuffer<256> {
  void handle_clock() {}
  void handle_start() {}
  void handle_continue() {}
  void handle_stop() {}
  void handle_active_sensing() {}
  void handle_system_reset() {}
  void handle_tune_request() {}

  void handle_sysex(const std::span<const uint8_t> sysex) {}
  void handle_sysex_overflow() {}

  void handle_song_select(const uint8_t song) {}

  void handle_midi_time_code(const uint8_t i) {}

  void handle_program_change(const uint8_t chan, const uint8_t pg) {}

  void handle_channel_pressure(const uint8_t chan, const uint8_t v) {}

  void handle_note_on(const uint8_t chan, const uint8_t note,
                      const uint8_t velocity) {}

  void handle_note_off(const uint8_t chan, const uint8_t note,
                       const uint8_t velocity) {}

  void handle_channel_mode(const uint8_t chan, const uint8_t cc,
                           const uint8_t val) {}

  void handle_control_change(const uint8_t chan, const uint8_t cc,
                             const uint8_t val) {}

  void handle_polyphonic_aftertouch(const uint8_t chan, const uint8_t note,
                                    const uint8_t val) {}

  void handle_pitch_bend(const uint8_t chan, const uint8_t lsb,
                         const uint8_t msb) {}

  void handle_song_position_pointer(const uint8_t lsb, const uint8_t msb) {}
};
