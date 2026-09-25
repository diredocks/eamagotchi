#include "Arduino.h"

#include "globals.hpp"
#include "icons.hpp"

#include "display.hpp"

// Staging frame published by the main loop and consumed by displayTask().
// Only ever touched under render_mutex.
static SemaphoreHandle_t render_mutex = nullptr;
static bool_t render_matrix_buffer[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
static bool_t render_icon_buffer[TAMA_ICON_NUM];
static volatile bool render_pending = false;
static volatile bool render_full_pending = false;

void drawIcon(int x, int y, int iconIdx, bool active, bool hideWhenInactive) {
  if (hideWhenInactive && !active) return;
  const unsigned char* bmp = (const unsigned char*)pgm_read_ptr(&icon_allArray[iconIdx]);
  // Bitmaps are inverted: 1=white, 0=black. Use fg=WHITE bg=BLACK to render correctly.
  display.drawBitmap(x, y, bmp, ICON_W, ICON_H, GxEPD_WHITE, GxEPD_BLACK);
  if (active && !hideWhenInactive) {
    // Draw 2px border outline with 2px padding around the icon
    display.drawRect(x - 2, y - 2, ICON_W + 4, ICON_H + 4, GxEPD_BLACK);
    display.drawRect(x - 3, y - 3, ICON_W + 6, ICON_H + 6, GxEPD_BLACK);
  }
}

void renderFrameContent(const bool_t matrix[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH],
                        const bool_t icons[TAMA_ICON_NUM]) {
  display.fillScreen(GxEPD_WHITE);

  // Draw top icon row (indices 0-3: feed, light, play, medi)
  for (int i = 0; i < 4; i++) {
    int ix = i * ICON_SLOT_W + (ICON_SLOT_W - ICON_W) / 2;
    drawIcon(ix, TOP_ICON_ROW_Y, i, icons[i]);
  }

  // Draw bottom icon row (indices 4-7: duck, meter, scold, call)
  // icon_call (index 7, i==3) is hidden when inactive
  for (int i = 0; i < 4; i++) {
    int ix = i * ICON_SLOT_W + (ICON_SLOT_W - ICON_W) / 2;
    bool hideWhenInactive = (i == 3); // index 7 = call
    drawIcon(ix, BOT_ICON_ROW_Y, i + 4, icons[i + 4], hideWhenInactive);
  }

  // Draw main LCD area (32x16 scaled by PIXEL_SCALE)
  for (int y = 0; y < TAMA_LCD_HEIGHT; y++) {
    for (int x = 0; x < TAMA_LCD_WIDTH; x++) {
      if (matrix[y][x]) {
        int px = LCD_OFFSET_X + x * PIXEL_SCALE;
        int py = LCD_OFFSET_Y + y * PIXEL_SCALE;
        // Draw filled rectangle with 1px gap for grid effect
        display.fillRect(px, py, PIXEL_SCALE - 1, PIXEL_SCALE - 1, GxEPD_BLACK);
      }
    }
  }
}

static void enqueueFrame(bool full) {
  if (!render_mutex) return;
  xSemaphoreTake(render_mutex, portMAX_DELAY);
  memcpy(render_matrix_buffer, matrix_buffer, sizeof(render_matrix_buffer));
  memcpy(render_icon_buffer, icon_buffer, sizeof(render_icon_buffer));
  if (full) render_full_pending = true;
  render_pending = true;
  xSemaphoreGive(render_mutex);
}

void renderScreen() { enqueueFrame(false); }
void renderScreenFull() { enqueueFrame(true); }
void renderScreenPartial() { enqueueFrame(false); }

void displayInit() {
  render_mutex = xSemaphoreCreateMutex();
}

void displayTask(void *param) {
  (void)param;
  static bool_t local_matrix[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH];
  static bool_t local_icons[TAMA_ICON_NUM];

  for (;;) {
    if (!render_pending) {
      vTaskDelay(pdMS_TO_TICKS(5));
      continue;
    }

    bool full;
    xSemaphoreTake(render_mutex, portMAX_DELAY);
    render_pending = false;
    full = render_full_pending;
    render_full_pending = false;
    memcpy(local_matrix, render_matrix_buffer, sizeof(local_matrix));
    memcpy(local_icons, render_icon_buffer, sizeof(local_icons));
    xSemaphoreGive(render_mutex);

    // Skip if nothing actually changed (full refreshes always run)
    bool changed = full;
    for (int y = 0; y < TAMA_LCD_HEIGHT && !changed; y++) {
      for (int x = 0; x < TAMA_LCD_WIDTH && !changed; x++) {
        if (local_matrix[y][x] != prev_matrix_buffer[y][x]) changed = true;
      }
    }
    for (int i = 0; i < TAMA_ICON_NUM && !changed; i++) {
      if (local_icons[i] != prev_icon_buffer[i]) changed = true;
    }
    if (!changed) continue;

    // Rate limit: don't refresh faster than the e-ink can handle
    unsigned long now = millis();
    if (now - last_screen_update_ms < TAMA_SCREEN_MIN_MS) {
      render_pending = true;
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }
    last_screen_update_ms = now;

    // Commit prev before drawing so changes made during the refresh are kept
    memcpy(prev_matrix_buffer, local_matrix, sizeof(local_matrix));
    memcpy(prev_icon_buffer, local_icons, sizeof(local_icons));

    if (full) display.setFullWindow();
    else display.setPartialWindow(0, 0, EPD_WIDTH, EPD_HEIGHT);
    display.firstPage();
    do {
      renderFrameContent(local_matrix, local_icons);
    } while (display.nextPage());
  }
}
