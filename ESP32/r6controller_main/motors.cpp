#include "motors.h"
#include "config.h"

// Convierte porcentaje (-100..100) a PWM con signo (-MAX_PWM..MAX_PWM).
// Cualquier valor distinto de 0 empieza en MIN_PWM para que el motor arranque.
static int percentToPwm(int percent) {
  percent = constrain(percent, -100, 100);
  if (percent == 0) return 0;
  int magnitude = MIN_PWM + (abs(percent) * (MAX_PWM - MIN_PWM)) / 100;
  return percent > 0 ? magnitude : -magnitude;
}

// Tabla del TB6612:  IN1=H IN2=L -> adelante | IN1=L IN2=H -> atrás | L L -> libre
static void setMotor(uint8_t pwmPin, uint8_t in1, uint8_t in2, int pwm) {
  if (pwm > 0)      { digitalWrite(in1, HIGH); digitalWrite(in2, LOW);  }
  else if (pwm < 0) { digitalWrite(in1, LOW);  digitalWrite(in2, HIGH); }
  else              { digitalWrite(in1, LOW);  digitalWrite(in2, LOW);  }
  ledcWrite(pwmPin, abs(pwm));
}

void motorsInit() {
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, HIGH);   // habilita el TB6612

  ledcAttach(PIN_PWMA, PWM_FREQ, PWM_RES);
  ledcAttach(PIN_PWMB, PWM_FREQ, PWM_RES);
  motorsStop();
}

void motorsDrive(int leftPercent, int rightPercent) {
  if (INVERT_LEFT)  leftPercent  = -leftPercent;
  if (INVERT_RIGHT) rightPercent = -rightPercent;
  setMotor(PIN_PWMA, PIN_AIN1, PIN_AIN2, percentToPwm(leftPercent));
  setMotor(PIN_PWMB, PIN_BIN1, PIN_BIN2, percentToPwm(rightPercent));
}

void motorsStop() {
  motorsDrive(0, 0);
}
