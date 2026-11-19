#include "gps_tracker.h"
#include "config.h"
#include <TinyGPSPlus.h>

static TinyGPSPlus gps;
static HardwareSerial gpsSerial(2);   // UART2

void gpsInit() {
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);
}

void gpsUpdate() {
  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  static unsigned long t = 0;
  if (millis() - t > 2000) {
    t = millis();
    Serial.printf("[gps] chars=%lu  frases_ok=%lu  sat=%u  fix=%s\n",
      gps.charsProcessed(), gps.sentencesWithFix(),
      gps.satellites.isValid() ? gps.satellites.value() : 0,
      gpsHasFix() ? "SI" : "NO");
  }
}

bool gpsHasFix() {
  return gps.location.isValid() && gps.location.age() < GPS_MAX_AGE_MS;
}

void gpsToJson(char* out, size_t outSize) {
  snprintf(out, outSize,
           "{\"t\":\"gps\",\"fix\":%s,\"lat\":%.6f,\"lon\":%.6f,\"sat\":%u,\"spd\":%.1f}",
           gpsHasFix() ? "true" : "false",
           gps.location.lat(),
           gps.location.lng(),
           gps.satellites.isValid() ? (unsigned)gps.satellites.value() : 0u,
           gps.speed.isValid() ? gps.speed.kmph() : 0.0);
}
