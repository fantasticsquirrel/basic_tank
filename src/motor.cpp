#include "motor.h"

#include <cstdlib>

void Motor::begin(uint16_t frequencyHz, uint8_t resolutionBits) {
  pinMode(pins_.in1, OUTPUT);
  pinMode(pins_.in2, OUTPUT);
  ledcSetup(pins_.pwmChannel, frequencyHz, resolutionBits);
  ledcAttachPin(pins_.pwm, pins_.pwmChannel);
  stop();
}

void Motor::write(int signedSpeed) {
  signedSpeed = constrain(signedSpeed, -255, 255);
  if (pins_.inverted) signedSpeed = -signedSpeed;
  if (signedSpeed == 0) {
    digitalWrite(pins_.in1, LOW);
    digitalWrite(pins_.in2, LOW);
  } else {
    digitalWrite(pins_.in1, signedSpeed > 0 ? HIGH : LOW);
    digitalWrite(pins_.in2, signedSpeed > 0 ? LOW : HIGH);
  }
  ledcWrite(pins_.pwmChannel, std::abs(signedSpeed));
}

