#pragma once

#include <cstdint>
#include <span>

namespace EmbeddedMidi {

class Parser {
  uint8_t run_stat_reg{};
  uint8_t buf{};
  bool third_byte_flag{};

public:
  template <typename T>
  constexpr void parse(this auto &self, const std::span<const uint8_t> stream,
                       T &&handler) {
    for (auto &d : stream) {
      self.parse(d, std::forward<T>(handler));
    }
  }

  template <typename T>
  constexpr void parse(this auto &self, const uint8_t raw_midi, T &&handler) {
    if (raw_midi & 0x80) {
      self.parse_status_byte(raw_midi, std::forward<T>(handler));
    } else {
      self.parse_data_byte(raw_midi, std::forward<T>(handler));
    }
  };

private:
  template <typename T>
  constexpr void handle_three_byte_message(const uint8_t third_byte,
                                           T &&handler) {
    // only one non-channel message with three bytes
    if (run_stat_reg == 0xf2) {
      handler.handle_song_position_pointer(buf, third_byte);
      return;
    }

    const auto status = run_stat_reg & 0xf0;
    const auto midi_channel = run_stat_reg & 0x0f;

    switch (status) {
    case 0x90:
      if (third_byte) {
        handler.handle_note_on(midi_channel, buf, third_byte);
        return;
      }
      // note on with 0 velocity is a note off
      [[fallthrough]];
    case 0x80: handler.handle_note_off(midi_channel, buf, third_byte); return;
    case 0xA0:
      handler.handle_polyphonic_aftertouch(midi_channel, buf, third_byte);
      return;
    case 0xB0:
      if (buf >= 120) {
        handler.handle_channel_mode(midi_channel, buf, third_byte);
      } else {
        handler.handle_control_change(midi_channel, buf, third_byte);
      }
      return;
    case 0xE0: handler.handle_pitch_bend(midi_channel, buf, third_byte); return;
    }
  }

  template <typename T>
  constexpr void handle_two_byte_channel_message(const uint8_t second_byte,
                                                 T &&handler) {
    const auto status = run_stat_reg & 0xf0;
    const auto midi_channel = run_stat_reg & 0x0f;

    switch (status) {
    case 0xC0: handler.handle_program_change(midi_channel, second_byte); return;
    case 0xD0:
      handler.handle_channel_pressure(midi_channel, second_byte);
      return;
    }
  }

  template <typename T>
  constexpr void parse_data_byte(this auto &self, const uint8_t input,
                                 T &&handler) {
    auto &t = static_cast<Parser &>(self);
    if (t.third_byte_flag) {
      t.third_byte_flag = {};
      t.handle_three_byte_message(input, std::forward<T>(handler));
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
      t.handle_two_byte_channel_message(input, std::forward<T>(handler));
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
      handler.handle_midi_time_code(input);
      t.run_stat_reg = {};
      return;
    case 0xF3:
      handler.handle_song_select(input);
      t.run_stat_reg = {};
      return;
    case 0xF0:
      self.handle_sysex_byte_internal(input, std::forward<T>(handler));
      return;
    }

    t.run_stat_reg = {};
  }

  template <typename T>
  constexpr void parse_status_byte(this auto &self, const uint8_t input,
                                   T &&handler) {
    // midi realtime always wins!
    auto &t = static_cast<Parser &>(self);

    switch (input) {
    case 0xf8: handler.handle_clock(); return;
    case 0xf9: handler.handle_undefined_realtime(input); return;
    case 0xfa: handler.handle_start(); return;
    case 0xfb: handler.handle_continue(); return;
    case 0xfc: handler.handle_stop(); return;
    case 0xfd: handler.handle_undefined_realtime(input); return;
    case 0xfe: handler.handle_active_sensing(); return;
    case 0xff: handler.handle_system_reset(); return;
    default:   break;
    }

    if (t.run_stat_reg == 0xf0) {
      self.handle_end_of_sysex_internal(input == 0xf7,
                                        std::forward<T>(handler));
    }

    t.run_stat_reg = input;
    t.third_byte_flag = {};

    switch (input) {
    case 0xf6: handler.handle_tune_request(); return;
    }
  }

  template <typename T>
  constexpr void handle_end_of_sysex_internal(bool valid, T &&handler) {
    handler.handle_end_of_sysex(valid);
  }

  template <typename T>
  constexpr void handle_sysex_byte_internal(uint8_t b, T &&handler) {
    handler.handle_sysex_byte(b);
  }
};

template <uint32_t size_> class ParserWithSysexBuffer : public Parser {
  friend class Parser;
  std::array<uint8_t, size_> sys{};
  uint32_t cnt{};

private:
  template <typename T>
  constexpr void handle_sysex_byte_internal(const uint8_t data, T &&) {
    if (cnt < size_) {
      sys[cnt++] = data;
      return;
    }

    ++cnt;
  }

  template <typename T>
  constexpr void handle_end_of_sysex_internal(const bool valid, T &&handler) {
    if (valid) {
      if (cnt > size_) {
        handler.handle_sysex_overflow(cnt - size_);
      } else {
        handler.handle_sysex(std::span<const uint8_t>{sys.data(), cnt});
      }
    }

    cnt = {};
  }
};

// helper handler structure.
// handles everything with a NOP
// you can inherit this class and override functions
struct DefaultHandler {
  constexpr void handle_undefined_realtime(const uint8_t b) {}
  constexpr void handle_clock() {}
  constexpr void handle_start() {}
  constexpr void handle_continue() {}
  constexpr void handle_stop() {}
  constexpr void handle_active_sensing() {}
  constexpr void handle_system_reset() {}
  constexpr void handle_tune_request() {}

  // this will only be called if you use Parser
  constexpr void handle_end_of_sysex(bool valid) {}
  // this will only be called if you use Parser
  constexpr void handle_sysex_byte(uint8_t b) {}

  // this will only be called if you use ParserWithSysexBuffer
  constexpr void handle_sysex_overflow(uint32_t amount) {}
  // this will only be called if you use ParserWithSysexBuffer
  constexpr void handle_sysex(std::span<const uint8_t> sysex) {}

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
