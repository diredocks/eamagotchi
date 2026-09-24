#ifndef _EAMA_DISPLAY_H_
#define _EAMA_DISPLAY_H_

#include <stdint.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_GFX.h>

#define EPD_CS 5
#define EPD_DC 17
#define EPD_RST 16
#define EPD_BUSY 4

void setupDisplay();

#endif
