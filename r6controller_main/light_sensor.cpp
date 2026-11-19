#include "light_sensor.h"
#include "config.h"

static int  level = 0;
static bool ledOn = false;

// Promedia varias lecturas para quitar ruido
static int readAverage() {
  long sum = 0;
  for (int i = 0; i < 8; i++) sum += analogRead(PIN_LDR);
  return sum / 8;
}

void lightInit() {
  analogReadResolution(8);                       // 0..255
  analogSetPinAttenuation(PIN_LDR, ADC_11db);     // rango completo 0..3.3 V
  pinMode(PIN_LIGHT_LED, OUTPUT);
  digitalWrite(PIN_LIGHT_LED, LOW);
}

void lightUpdate() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead < LIGHT_READ_INTERVAL_MS) return;
  lastRead = millis();

  level = readAverage();

  if (!ledOn && level < LIGHT_ON_BELOW) {
    ledOn = true;
    digitalWrite(PIN_LIGHT_LED, HIGH);
  } else if (ledOn && level > LIGHT_OFF_ABOVE) {
    ledOn = false;
    digitalWrite(PIN_LIGHT_LED, LOW);
  }

  if (LIGHT_DEBUG) {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
      lastPrint = millis();
      Serial.printf("[luz] lectura=%d  LED=%s\n", level, ledOn ? "ON" : "OFF");
    }
  }
}

int lightLevel()  { return level; }
bool lightLedOn() { return ledOn; }