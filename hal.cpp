#include "globals.hpp"
#include "config.hpp"

#include "hal.hpp"
#include "display.hpp"

#include <Arduino.h>

static volatile bool btn_interrupt_flags[2] = {false, false};

void IRAM_ATTR btnISR_UP() { btn_interrupt_flags[0] = true; }
void IRAM_ATTR btnISR_DN() { btn_interrupt_flags[1] = true; }

static bool_t hal_is_log_enabled(log_level_t level) {
  return (level == LOG_ERROR) ? 1 : 0;
}

static void hal_log(log_level_t level, char *buff, ...) {
  if (level != LOG_ERROR) return;
  va_list args;
  va_start(args, buff);
  char msg[128];
  vsnprintf(msg, sizeof(msg), buff, args);
  va_end(args);
  Serial.print("[EAMA:ERR] ");
  Serial.println(msg);
}

static void hal_sleep_until(timestamp_t ts) {
  // TODO: if (fast_forwarding) return;
  while (true) {
    timestamp_t now = (timestamp_t)micros();
    int32_t remaining = (int32_t)(ts - now);
    if (remaining <= 0 || remaining >= 1000000) break;
    pollButtons();
  }
}

static timestamp_t hal_get_timestamp(void) {
  return (timestamp_t)micros();
}

static void hal_update_screen(void) {
  // TODO: if (!fast_forwarding) renderScreen();
  renderScreen();
}

static void hal_set_lcd_matrix(u8_t x, u8_t y, bool_t val) {
  if (x < TAMA_LCD_WIDTH && y < TAMA_LCD_HEIGHT) {
    matrix_buffer[y][x] = val;
  }
}

static void hal_set_lcd_icon(u8_t icon, bool_t val) {
  if (icon < TAMA_ICON_NUM) {
    icon_buffer[icon] = val;
  }
}

static void hal_halt(void) {}
static void hal_set_frequency(u32_t freq) { (void)freq; }
static void hal_play_frequency(bool_t en) { (void)en; }
static void* hal_malloc(u32_t size) { return malloc(size); }
static void hal_free(void *ptr) { free(ptr); }

void pollButtons() {
  unsigned long now = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    // check if the ISR flagged a press (instant capture)
    bool isr_pressed = btn_interrupt_flags[i];
    if (isr_pressed) {
      btn_interrupt_flags[i] = false;
    }

    bool current = (digitalRead(buttons[i].pin) == LOW) || isr_pressed;

    if (current != buttons[i].stable_state) {
      // reading differs from stable state — track as pending
      if (current != buttons[i].pending_state) {
        // new candidate, start the debounce timer
        buttons[i].pending_state = current;
        buttons[i].pending_since = now;
      } else if (now - buttons[i].pending_since >= BTN_DEBOUNCE_MS) {
        // candidate held for debounce period — accept it
        buttons[i].stable_state = current;
        tamalib_set_button(buttons[i].tama_btn,
                           current ? BTN_STATE_PRESSED : BTN_STATE_RELEASED);
        if (current) {
          last_button_ms = now; // reset idle timeout
        }
      }
    } else {
      // reading matches stable state — reset pending
      buttons[i].pending_state = current;
    }
  }
}

static int hal_handler(void) {
  pollButtons();
  return 0; // Return 0 to keep running
}

hal_t hal = {
  .malloc         = &hal_malloc,
  .free           = &hal_free,
  .halt           = &hal_halt,
  .is_log_enabled = &hal_is_log_enabled,
  .log            = &hal_log,
  .sleep_until    = &hal_sleep_until,
  .get_timestamp  = &hal_get_timestamp,
  .update_screen  = &hal_update_screen,
  .set_lcd_matrix = &hal_set_lcd_matrix,
  .set_lcd_icon   = &hal_set_lcd_icon,
  .set_frequency  = &hal_set_frequency,
  .play_frequency = &hal_play_frequency,
  .handler        = &hal_handler,
};
