#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <GxEPD2_BW.h>

#include "config.hpp"

extern "C" {
  #include "lib/tamalib/tamalib.h"
}

extern GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display;

extern bool_t matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
extern bool_t icon_buffer[TAMA_ICON_NUM];
extern bool_t prev_matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
extern bool_t prev_icon_buffer[TAMA_ICON_NUM];

extern unsigned long last_button_ms;
extern unsigned long last_screen_update_ms;

struct ButtonState {
  uint8_t pin;
  button_t tama_btn;
  bool stable_state; // Last confirmed/registered state
  bool pending_state; // Candidate state being debounced
  unsigned long pending_since; // millis() when pending_state was first seen
};
extern ButtonState buttons[];
extern const int   NUM_BUTTONS;

#define ROM_SIZE 6144
extern u12_t rom_data[ROM_SIZE];

#endif
