// =====================================================================
//  config.h  —  TODOS los ajustes del carro en un solo lugar
// =====================================================================
#pragma once
#include <Arduino.h>

// ---------- WiFi (la ESP32 crea esta red) ----------
constexpr const char* WIFI_SSID     = "R6Controller";
constexpr const char* WIFI_PASS     = "r6controller";   // mínimo 8 caracteres
constexpr uint8_t     WIFI_CHANNEL  = 6;
constexpr uint8_t     WIFI_MAX_CLIENTS = 4;
// IP del carro: 192.168.4.1  (se define en comms.cpp)

// ---------- Pines TB6612FNG ----------
constexpr uint8_t PIN_PWMA = 25;
constexpr uint8_t PIN_AIN1 = 26;
constexpr uint8_t PIN_AIN2 = 27;
constexpr uint8_t PIN_PWMB = 14;
constexpr uint8_t PIN_BIN1 = 33;
constexpr uint8_t PIN_BIN2 = 32;
constexpr uint8_t PIN_STBY = 13;

// ---------- Motores ----------
constexpr uint32_t PWM_FREQ = 20000;   // 20 kHz: silencioso
constexpr uint8_t  PWM_RES  = 8;       // 0..255
constexpr int MIN_PWM = 60;    // PWM mínimo para que el N20 arranque
constexpr int MAX_PWM = 255;   // N20 de 6 V con baterías 2S -> usa ~180
constexpr bool INVERT_LEFT  = false;   // true si el motor izquierdo gira al revés
constexpr bool INVERT_RIGHT = true;

// ---------- Manejo ----------
constexpr int      JOY_DEADZONE = 8;      // ignora movimientos pequeños del joystick
constexpr uint32_t FAILSAFE_MS  = 400;    // sin órdenes este tiempo -> parar

// ---------- GPS ----------
constexpr uint8_t  PIN_GPS_RX = 16;   // aquí va el TX del GPS
constexpr uint8_t  PIN_GPS_TX = 17;   // aquí va el RX del GPS
constexpr uint32_t GPS_BAUD   = 9600;
constexpr uint32_t GPS_SEND_INTERVAL_MS = 1000;   // cada cuánto se envía a la app
constexpr uint32_t GPS_MAX_AGE_MS       = 3000;   // dato más viejo que esto = sin fix

// ---------- Sensor de luz (fotorresistencia) ----------
constexpr uint8_t  PIN_LDR = 34;            // debe ser ADC1 (32-39) porque usamos WiFi
constexpr uint8_t  PIN_LIGHT_LED = 23;
constexpr int      LIGHT_ON_BELOW  = 1200;  // si la lectura baja de aquí -> prende el LED
constexpr int      LIGHT_OFF_ABOVE = 1500;  // si sube de aquí -> apaga el LED
constexpr uint32_t LIGHT_READ_INTERVAL_MS = 100;
constexpr bool     LIGHT_DEBUG = true;      // imprime las lecturas en el Monitor Serie para calibrar