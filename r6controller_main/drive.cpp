#include "drive.h"
#include "motors.h"
#include "config.h"

// Se escriben desde la tarea del WebSocket y se leen en loop()
static volatile int targetLeft  = 0;
static volatile int targetRight = 0;
static volatile unsigned long lastCommandMs = 0;

static int applyDeadzone(int v) {
  return abs(v) < JOY_DEADZONE ? 0 : v;
}

// Tracción diferencial:
//   izquierda = avance + giro
//   derecha   = avance - giro
void driveSetJoystick(int x, int y) {
  x = applyDeadzone(constrain(x, -100, 100));
  y = applyDeadzone(constrain(y, -100, 100));

  int left  = y + x;
  int right = y - x;

  // Si alguno pasa de 100, se escalan los dos para mantener la curva
  int biggest = max(abs(left), abs(right));
  if (biggest > 100) {
    left  = left  * 100 / biggest;
    right = right * 100 / biggest;
  }

  targetLeft  = left;
  targetRight = right;
  lastCommandMs = millis();
}

void driveStop() {
  targetLeft = 0;
  targetRight = 0;
}

void driveUpdate() {
  // Failsafe: si la app deja de mandar órdenes, el carro se detiene
  if (millis() - lastCommandMs > FAILSAFE_MS) driveStop();
  motorsDrive(targetLeft, targetRight);
}
