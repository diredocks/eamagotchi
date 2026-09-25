#ifndef _CONFIG_H_
#define _CONFIG_H_

// button
#define BTN_UP_PIN 14 // upper button on device (black)
#define BTN_DN_PIN 12 // lower button on device (white)
#define BTN_DEBOUNCE_MS 50

// tamalib - emulator settings
#define TAMA_DISPLAY_FRAMERATE 6
#define TAMA_TIMESTAMP_FREQ 1000000
#define TAMA_SCREEN_MIN_MS 300

// display
#define EPD_CS 5
#define EPD_DC 17
#define EPD_RST 16
#define EPD_BUSY 4

#define EPD_WIDTH 400
#define EPD_HEIGHT 300

// display scaling
#define TAMA_LCD_WIDTH 32
#define TAMA_LCD_HEIGHT 16
#define TAMA_ICON_NUM 8

#define PIXEL_SCALE 12.5
#define LCD_SCALED_W (TAMA_LCD_WIDTH  * PIXEL_SCALE) // 400
#define LCD_SCALED_H (TAMA_LCD_HEIGHT * PIXEL_SCALE) // 200
#define LCD_OFFSET_X ((EPD_WIDTH  - LCD_SCALED_W) / 2)
#define LCD_OFFSET_Y 50

// icons
#define ICON_W           40
#define ICON_H           24
#define ICON_SLOT_W      (EPD_WIDTH / 4)
#define TOP_ICON_ROW_Y   ((LCD_OFFSET_Y - ICON_H) / 2)
#define BOT_ICON_ROW_Y   (LCD_OFFSET_Y + LCD_SCALED_H + ((EPD_HEIGHT - LCD_OFFSET_Y - LCD_SCALED_H - ICON_H) / 2))

#endif
