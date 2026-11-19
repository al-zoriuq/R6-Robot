// =====================================================================
//  config.h  —  Ajustes de la ESP32-CAM
// =====================================================================
#pragma once
#include <Arduino.h>
#include "esp_camera.h"

// ---------- Modo de prueba ----------
// true  -> la ESP32-CAM crea ella misma la red "R6Controller" (para probar SIN la ESP32 principal)
// false -> modo normal: se conecta a la red que crea la ESP32 principal
constexpr bool STANDALONE_TEST = false;

// ---------- WiFi (red creada por la ESP32 principal) ----------
constexpr const char* WIFI_SSID = "R6Controller";
constexpr const char* WIFI_PASS = "r6controller";
// IP fija de la cámara: 192.168.4.2 (se define en wifi_link.cpp)

// ---------- Video ----------
constexpr uint16_t   STREAM_PORT  = 81;                 // http://192.168.4.2:81/stream
constexpr framesize_t FRAME_SIZE  = FRAMESIZE_VGA;      // 640x480. Más fluido: FRAMESIZE_QVGA
constexpr int        JPEG_QUALITY = 12;                 // 10-20: mayor = menos calidad, más fluido
constexpr bool       FLIP_VERTICAL   = false;           // true si la imagen sale al revés
constexpr bool       MIRROR_HORIZONTAL = false;

// ---------- Pines cámara AI-Thinker ----------
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM   0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM     5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22
