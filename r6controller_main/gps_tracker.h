// =====================================================================
//  gps_tracker.h  —  Lectura del módulo GPS
// =====================================================================
#pragma once
#include <Arduino.h>

// Abre el puerto serie del GPS. Llamar una vez en setup().
void gpsInit();

// Lee los datos pendientes del GPS. Llamar en cada loop().
void gpsUpdate();

// true si hay posición válida y reciente.
bool gpsHasFix();

// Escribe en 'out' el mensaje JSON para la app:
// {"t":"gps","fix":true,"lat":..,"lon":..,"sat":..,"spd":..}
void gpsToJson(char* out, size_t outSize);
