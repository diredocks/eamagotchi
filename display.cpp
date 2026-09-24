#include "display.hpp"

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT>
  display(GxEPD2_420_M01(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

void setupDisplay() {
  display.init(115200, true, 10, false);
  display.setRotation(0);
  Serial.println("[eama] display initialized");
}
