// =====================================================================
//  motors.h  —  Control a bajo nivel del puente H TB6612FNG
// =====================================================================
#pragma once

// Configura pines y PWM. Llamar una vez en setup().
void motorsInit();

// Velocidad de cada rueda en porcentaje: -100 (atrás) .. 0 .. 100 (adelante)
void motorsDrive(int leftPercent, int rightPercent);

// Detiene ambos motores.
void motorsStop();
