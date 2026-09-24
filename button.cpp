#include "button.hpp"

OneButton btnUp;
OneButton btnDown;

void setupButtons() {
  btnUp.setup(BUTTON_UP, INPUT_PULLUP, true);
  btnDown.setup(BUTTON_DOWN, INPUT_PULLUP, true);

  btnUp.attachClick([]() {
    Serial.println("[eama] clicked btn up");
  });
  btnDown.attachClick([]() {
    Serial.println("[eama] clicked btn down");
  });
  btnUp.attachLongPressStart([]() {
    Serial.println("[eama] pressed btn up");
  });
  btnDown.attachLongPressStart([]() {
    Serial.println("[eama] pressed btn down");
  });

  Serial.println("[eama] button initialized");
}

void tickButtons() {
  btnUp.tick();
  btnDown.tick();
}
