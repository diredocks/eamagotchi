#include <button.hpp>
#include <display.hpp>

void setupBoot() {
  Serial.begin(115200);
  Serial.println("\n[eama] bootup...");
}

void setup() {
  setupBoot();
  setupButtons();
  setupDisplay();
}

void loop() {
  tickButtons();
}
