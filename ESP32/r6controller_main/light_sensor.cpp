#include "light_sensor.h"
#include "config.h"

static int  level = 0;
static bool ledOn = false;

void lightInit() {
  analogReadResolution(12);   // 0..4095
  pinMode(PIN_LIGHT_LED, OUTPUT);
}

void lightUpdate() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead < 500) return;
  lastRead = millis();

  level = analogRead(PIN_LDR);
  ledOn = (level < LIGHT_ON_BELOW);
  digitalWrite(PIN_LIGHT_LED, ledOn);

  Serial.printf("[luz] lectura=%d  LED=%s\n", level, ledOn ? "ON" : "OFF");
}

int lightLevel()  { return level; }
bool lightLedOn() { return ledOn; }