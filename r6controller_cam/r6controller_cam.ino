/*
 * =====================================================================
 *  R6Controller - ESP32-CAM (AI-Thinker)
 * =====================================================================
 *  Archivos:
 *    config.h              -> WiFi, resolución, calidad y pines
 *    cam_sensor.h/.cpp     -> inicialización del sensor
 *    stream_server.h/.cpp  -> servidor de video MJPEG
 *    wifi_link.h/.cpp      -> conexión a la red del carro
 *
 *  Video en: http://192.168.4.2:81/stream
 *  Placa: "AI Thinker ESP32-CAM" (Arduino core ESP32 3.x), PSRAM habilitada
 * =====================================================================
 */
#include "config.h"
#include "cam_sensor.h"
#include "stream_server.h"
#include "wifi_link.h"

void setup() {
  Serial.begin(115200);

  if (!cameraInit()) {
    Serial.println("[cam] Error al iniciar la cámara. Reiniciando...");
    delay(2000);
    ESP.restart();
  }

  wifiConnect();
  streamServerStart();
}

void loop() {
  wifiKeepAlive();
  delay(500);
}
