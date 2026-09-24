#include <GxEPD2_BW.h>

#include "config.hpp"

extern "C" {
  #include "lib/tamalib/tamalib.h"
}

#include "globals.hpp"
#include "hal.hpp"

OneButton button_up;
OneButton button_dn;

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT>
  display(GxEPD2_420_M01(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

bool_t matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
bool_t icon_buffer[TAMA_ICON_NUM];
bool_t prev_matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
bool_t prev_icon_buffer[TAMA_ICON_NUM];

void setup() {
  Serial.begin(115200);
  Serial.println("\n[EAMA] now live");

  // TODO: determine wake cause

  // initialize button pins
  button_up.setup(BTN_UP_PIN, INPUT_PULLUP, true);
  button_dn.setup(BTN_DN_PIN, INPUT_PULLUP, true);
  Serial.println("[EAMA] buttons initialized");

  // TODO: reset states by double clicking down button
  
  // initialize e-ink display
  // TODO: use initial=false on timer wakes to avoid a full hardware clear
  display.init(115200);
  display.setRotation(0);
  Serial.println("[EAMA] display initialized");

  // TODO: load rom from whatever? (might include it in firmware)

  // initialize tamalib
  tamalib_register_hal(&hal);
  tamalib_set_framerate(TAMA_DISPLAY_FRAMERATE);

  // TODO: tamalib_init(rom_data, NULL, TAMA_TIMESTAMP_FREQ)
  Serial.println("[EAMA] tamalib initialized");

  // clear previous buffers so first frame always draws
  memset(prev_matrix_buffer, 0xFF, sizeof(prev_matrix_buffer));
  memset(prev_icon_buffer, 0xFF, sizeof(prev_icon_buffer));

  // TODO: load saved state and fast-forward if waking from deep sleep
  // TODO: render the screen based on wake cause
  
  // entering interactive mode
  // TODO: last_x variables for entering deep sleep
  
  Serial.println("[EAMA] entering interactive mode");
}

static timestamp_t loop_screen_ts = 0;

void loop() {
  // run emulation step
  // TODO: tamalib_step();

  // tick buttons
  tickButtons();

  // update screen at configured framerate
  timestamp_t ts = (timestamp_t)micros();
  if (ts - loop_screen_ts >= (timestamp_t)(TAMA_TIMESTAMP_FREQ / TAMA_DISPLAY_FRAMERATE)) {
    loop_screen_ts = ts;
    // TODO: renderScreen();
  }

  // TODO: check idle timeout
}
