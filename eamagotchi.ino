#include <GxEPD2_BW.h>

#include "config.hpp"
#include "rom.hpp"

extern "C" {
  #include "lib/tamalib/tamalib.h"
}

#include "globals.hpp"
#include "hal.hpp"
#include "display.hpp"

OneButton button_up;
OneButton button_dn;

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT>
  display(GxEPD2_420_M01(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

bool_t matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
bool_t icon_buffer[TAMA_ICON_NUM];
bool_t prev_matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
bool_t prev_icon_buffer[TAMA_ICON_NUM];

unsigned long last_screen_update_ms = 0;
unsigned long emu_btn_release_ms[3] = {0, 0, 0};

static void emuPulse(button_t btn) {
  // last_button_ms = millis();
  tamalib_set_button(btn, BTN_STATE_PRESSED);
  emu_btn_release_ms[btn] = millis() + BTN_EMU_HOLD_MS;
}

static void emuHold(button_t btn) {
  // last_button_ms = millis();
  emu_btn_release_ms[btn] = 0;
  tamalib_set_button(btn, BTN_STATE_PRESSED);
}

static void emuRelease(button_t btn) {
  emu_btn_release_ms[btn] = 0;
  tamalib_set_button(btn, BTN_STATE_RELEASED);
}

u12_t rom_data[ROM_SIZE];

static void loadRomFromProgmem() {
  for (int i = 0; i < ROM_SIZE / 2; i++) {
    uint8_t b0 = pgm_read_byte(&g_program[i * 3]);
    uint8_t b1 = pgm_read_byte(&g_program[i * 3 + 1]);
    uint8_t b2 = pgm_read_byte(&g_program[i * 3 + 2]);
    rom_data[i * 2]     = ((uint16_t)b0 << 4) | (b1 >> 4);
    rom_data[i * 2 + 1] = ((uint16_t)(b1 & 0x0F) << 8) | b2;
  }
  Serial.println("[EAMA] loaded ROM");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[EAMA] now live");

  // TODO: determine wake cause

  // initialize button pins
  button_up.setup(BTN_UP_PIN, INPUT_PULLUP, true);
  button_up.attachClick([]() { emuPulse(BTN_MIDDLE); });
  button_up.attachLongPressStart([]() { emuHold(BTN_MIDDLE); });
  button_up.attachLongPressStop([]() { emuRelease(BTN_MIDDLE); });
  button_dn.setup(BTN_DN_PIN, INPUT_PULLUP, true);
  button_dn.attachClick([]() { emuPulse(BTN_RIGHT); });
  button_dn.attachDoubleClick([]() { emuPulse(BTN_LEFT); });
  button_dn.attachLongPressStart([]() { emuHold(BTN_RIGHT); });
  button_dn.attachLongPressStop([]() { emuRelease(BTN_RIGHT); });
  Serial.println("[EAMA] buttons initialized");

  // TODO: reset states by double clicking down button
  
  // initialize e-ink display
  // TODO: use initial=false on timer wakes to avoid a full hardware clear
  display.init(115200);
  display.setRotation(0);
  Serial.println("[EAMA] display initialized");

  // load ROM from PROGMEM
  loadRomFromProgmem();

  // initialize tamalib
  tamalib_register_hal(&hal);
  tamalib_set_framerate(TAMA_DISPLAY_FRAMERATE);
  if (tamalib_init(rom_data, NULL, TAMA_TIMESTAMP_FREQ)) {
    Serial.println("[EAMA] ERROR: tamalib init failed");
    while (1) { delay(1000); }
  }
  Serial.println("[EAMA] tamalib initialized");
  delay(1000);

  // clear previous buffers so first frame always draws
  memset(prev_matrix_buffer, 0xFF, sizeof(prev_matrix_buffer));
  memset(prev_icon_buffer, 0xFF, sizeof(prev_icon_buffer));

  // TODO: load saved state and fast-forward if waking from deep sleep
  // TODO: render the screen based on wake cause
  
  // entering interactive mode
  // TODO: last_x variables for entering deep sleep
  last_screen_update_ms = 0;
  
  Serial.println("[EAMA] entering interactive mode");
}

static timestamp_t loop_screen_ts = 0;

void loop() {
  // run emulation step
  tamalib_step();

  // tick buttons
  tickButtons();

  // update screen at configured framerate
  timestamp_t ts = (timestamp_t)micros();
  if (ts - loop_screen_ts >= (timestamp_t)(TAMA_TIMESTAMP_FREQ / TAMA_DISPLAY_FRAMERATE)) {
    loop_screen_ts = ts;
    renderScreen();
  }

  // TODO: check idle timeout
}
