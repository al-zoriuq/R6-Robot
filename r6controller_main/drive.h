// =====================================================================
//  drive.h  —  Lógica de manejo: joystick -> ruedas, y failsafe
// =====================================================================
#pragma once

// Recibe el joystick de la app. x = giro, y = avance (ambos -100..100).
void driveSetJoystick(int x, int y);

// Detiene el carro de inmediato (p. ej. al desconectarse la app).
void driveStop();

// Llamar en cada loop(): aplica el failsafe y actualiza los motores.
void driveUpdate();
