// =====================================================================
//  comms.h  —  WiFi (punto de acceso) + WebSocket con la app
//
//  Protocolo:
//    App  -> ESP32 :  "J,x,y"   joystick, x,y en -100..100 (cada ~80 ms)
//    ESP32 -> App  :  JSON con el GPS (ver gps_tracker.h)
// =====================================================================
#pragma once
#include <Arduino.h>

// Crea la red WiFi e inicia el servidor WebSocket en ws://192.168.4.1/ws
void commsInit();

// Envía un texto a todas las apps conectadas.
void commsBroadcast(const char* text);

// true si hay al menos una app conectada.
bool commsHasClients();

// Limpieza periódica de conexiones cerradas. Llamar de vez en cuando.
void commsCleanup();
