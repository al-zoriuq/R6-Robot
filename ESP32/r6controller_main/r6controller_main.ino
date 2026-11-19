/*
 * =====================================================================
 *  R6Controller - ESP32 principal
 * =====================================================================
 *  Archivos:
 *    config.h          -> pines, WiFi y ajustes (edita aquí casi todo)
 *    motors.h/.cpp     -> puente H TB6612 (sentido y PWM de cada motor)
 *    drive.h/.cpp      -> joystick -> velocidad de cada rueda + failsafe
 *    gps_tracker.h/.cpp-> lectura del GPS y mensaje JSON
 *    comms.h/.cpp      -> red WiFi + WebSocket con la app
 *
 *  Placa: "ESP32 Dev Module" (Arduino core ESP32 3.x)
 *  Librerías: ESPAsyncWebServer + AsyncTCP (de ESP32Async), TinyGPSPlus
 * =====================================================================
 */
#include "config.h"
#include "motors.h"
#include "drive.h"
#include "gps_tracker.h"
#include "comms.h"
#include "light_sensor.h"

void setup() {
  Serial.begin(115200);
  motorsInit();
  gpsInit();
  lightInit();
  commsInit();
}

void loop() {
  gpsUpdate();     // lee datos del GPS
  driveUpdate();   // failsafe + aplica velocidad a los motores
  lightUpdate();

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= GPS_SEND_INTERVAL_MS) {
    lastSend = millis();
    if (commsHasClients()) {
      char json[160];
      gpsToJson(json, sizeof(json));
      commsBroadcast(json);
    }
    commsCleanup();
  }

  delay(5);
}
