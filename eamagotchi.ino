#include <button.hpp>

void setupBoot() {
  Serial.begin(115200);
  Serial.println("\n[eama] bootup...");
}

void setup() {
  setupBoot();
  setupButtons();
}

void loop() {
  tickButtons();
}
