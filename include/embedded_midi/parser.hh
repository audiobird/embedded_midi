#pragma once

#include <cstdint>
#include <span>

namespace EmbeddedMidi {

class Parser {
  uint8_t run_stat_reg{};
  uint8_t buf{};
  bool third_byte_flag{};

public:
  constexpr void parse(this auto &self, const std::span<const uint8_t> stream) {
    for (auto &d : stream) {
      self.parse(d);
    }
  }

  constexpr void parse(this auto &self, const uint8_t raw_midi) {
    if (raw_midi & 0x80) {
      self.parse_status_byte(raw_midi);
    } else {
      self.parse_data_byte(raw_midi);
    }
  };

private:
  constexpr void handle_three_byte_message(this auto &self,
                                           const uint8_t third_byte) {
    auto &t = static_cast<Parser &>(self);
    // only one non-channel message with three bytes
    if (t.run_stat_reg == 0xf2) {
      self.handle_song_position_pointer(t.buf, third_byte);
      return;
    }

    const auto status = t.run_stat_reg & 0xf0;
    const auto midi_channel = t.run_stat_reg & 0x0f;

    switch (status) {
    case 0x90:
      if (third_byte) {
        self.handle_note_on(midi_channel, t.buf, third_byte);
        return;
      }
      // note on with 0 velocity is a note off
      [[fallthrough]];
    case 0x80:
      self.handle_note_off(midi_channel, t.buf, third_byte);
      return;
    case 0xA0:
      self.handle_polyphonic_aftertouch(midi_channel, t.buf, third_byte);
      return;
    case 0xB0:
      if (t.buf >= 120) {
        self.handle_channel_mode(midi_channel, t.buf, third_byte);
      } else {
        self.handle_control_change(midi_channel, t.buf, third_byte);
      }
      return;
    case 0xE0:
      self.handle_pitch_bend(midi_channel, t.buf, third_byte);
      return;
    }
  }

  constexpr void handle_two_byte_channel_message(this auto &self,
                                                 const uint8_t second_byte) {
    auto &t = static_cast<Parser &>(self);

    const auto status = t.run_stat_reg & 0xf0;
    const auto midi_channel = t.run_stat_reg & 0x0f;

    switch (status) {
    case 0xC0:
      self.handle_program_change(midi_channel, second_byte);
      return;
    case 0xD0:
      self.handle_channel_pressure(midi_channel, second_byte);
      return;
    }
  }

  constexpr void parse_data_byte(this auto &self, const uint8_t input) {
    auto &t = static_cast<Parser &>(self);
    if (t.third_byte_flag) {
      t.third_byte_flag = {};
      self.handle_three_byte_message(input);
      return;
    }

    if (!t.run_stat_reg) {
      return;
    }

    // two byte channel messages
    if (t.run_stat_reg < 0xC0) {
      t.third_byte_flag = true;
      t.buf = input;
      return;
    }

    if (t.run_stat_reg < 0xE0) {
      self.handle_two_byte_channel_message(input);
      return;
    }

    if (t.run_stat_reg < 0xF0) {
      t.third_byte_flag = true;
      t.buf = input;
      return;
    }

    if (t.run_stat_reg == 0xF2) {
      t.run_stat_reg = {};
      t.third_byte_flag = true;
      t.buf = input;
      return;
    }

    switch (t.run_stat_reg) {
    case 0xF1:
      self.handle_midi_time_code(input);
      t.run_stat_reg = {};
      return;
    case 0xF3:
      self.handle_song_select(input);
      t.run_stat_reg = {};
      return;
    case 0xF0:
      self.handle_sysex_byte(input);
      return;
    }

    t.run_stat_reg = {};
  }

  constexpr void parse_status_byte(this auto &self, const uint8_t input) {
    auto &t = static_cast<Parser &>(self);

    // midi realtime always wins!
    switch (input) {
    case 0xf8:
      self.handle_clock();
      return;
    case 0xf9:
      self.handle_undefined_realtime(input);
      return;
    case 0xfa:
      self.handle_start();
      return;
    case 0xfb:
      self.handle_continue();
      return;
    case 0xfc:
      self.handle_stop();
      return;
    case 0xfd:
      self.handle_undefined_realtime(input);
      return;
    case 0xfe:
      self.handle_active_sensing();
      return;
    case 0xff:
      self.handle_system_reset();
      return;
    default:
      break;
    }

    if (t.run_stat_reg == 0xf0) {
      self.handle_end_of_sysex(input == 0xf7);
    }

    t.run_stat_reg = input;
    t.third_byte_flag = {};

    switch (input) {
    case 0xf6:
      self.handle_tune_request();
      return;
    }
  }

  constexpr void handle_undefined_realtime(const uint8_t b) {}
  constexpr void handle_clock() {}
  constexpr void handle_start() {}
  constexpr void handle_continue() {}
  constexpr void handle_stop() {}
  constexpr void handle_active_sensing() {}
  constexpr void handle_system_reset() {}
  constexpr void handle_tune_request() {}

  constexpr void handle_song_select(const uint8_t song) {}
  constexpr void handle_midi_time_code(const uint8_t i) {}

  constexpr void handle_program_change(const uint8_t chan, const uint8_t pg) {}

  constexpr void handle_channel_pressure(const uint8_t chan, const uint8_t v) {}

  constexpr void handle_note_on(const uint8_t chan, const uint8_t note,
                                const uint8_t velocity) {}

  constexpr void handle_note_off(const uint8_t chan, const uint8_t note,
                                 const uint8_t velocity) {}

  constexpr void handle_control_change(const uint8_t chan, const uint8_t cc,
                                       const uint8_t val) {}
  constexpr void handle_channel_mode(const uint8_t chan, const uint8_t cc,
                                     const uint8_t val) {}

  constexpr void handle_polyphonic_aftertouch(const uint8_t chan,
                                              const uint8_t note,
                                              const uint8_t val) {}

  constexpr void handle_pitch_bend(const uint8_t chan, const uint8_t lsb,
                                   const uint8_t msb) {}

  constexpr void handle_song_position_pointer(const uint8_t lsb,
                                              const uint8_t msb) {}
};

} // namespace EmbeddedMidi
