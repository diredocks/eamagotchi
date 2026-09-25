#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

#include "globals.hpp"

// Draw a single 40x24 icon at (x, y).
// If hideWhenInactive is true, the bitmap is skipped when not active
// (used for icon_call which should only appear during an incoming call).
void drawIcon(int x, int y, int iconIdx, bool active, bool hideWhenInactive = false);

// Render the given frame (icons + LCD pixels) into the current GxEPD2 page.
void renderFrameContent(const bool_t matrix[TAMA_LCD_HEIGHT][TAMA_LCD_WIDTH],
                        const bool_t icons[TAMA_ICON_NUM]);

// Queue a non-blocking render request. The actual e-ink refresh happens in
// displayTask() on the other core, so the caller never blocks on the panel.
void renderScreen();

// Queue a full refresh (clears ghosting).
void renderScreenFull();

// Queue a partial refresh (fast but may ghost).
void renderScreenPartial();

// Create the render mutex. Call once from setup() before creating displayTask().
void displayInit();

// FreeRTOS task that performs the blocking e-ink refreshes.
void displayTask(void *param);

#endif
