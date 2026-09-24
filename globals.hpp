#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <OneButton.h>
#include <GxEPD2_BW.h>

#include "config.hpp"

extern "C" {
  #include "lib/tamalib/tamalib.h"
}

extern OneButton button_up;
extern OneButton button_dn;
extern GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display;

extern bool_t matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
extern bool_t icon_buffer[TAMA_ICON_NUM];
extern bool_t prev_matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
extern bool_t prev_icon_buffer[TAMA_ICON_NUM];

#define ROM_SIZE 6144
extern u12_t rom_data[ROM_SIZE];

#endif
