// =====================================================================
//  light_sensor.h  —  Fotorresistencia + LED automático
// =====================================================================
#pragma once

// Configura el ADC y el pin del LED. Llamar una vez en setup().
void lightInit();

// Lee la luz y prende/apaga el LED. Llamar en cada loop().
void lightUpdate();

// Última lectura (0 = oscuro total, 4095 = mucha luz).
int lightLevel();

// true si el LED está encendido.
bool lightLedOn();