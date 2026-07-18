#pragma once

#include <Arduino.h>

struct MotorPins {
  uint8_t in1;
  uint8_t in2;
  uint8_t pwm;
  uint8_t pwmChannel;
  bool inverted;
};

class Motor {
 public:
  explicit Motor(MotorPins pins) : pins_(pins) {}
  void begin(uint16_t frequencyHz, uint8_t resolutionBits);
  void write(int signedSpeed);
  void stop() { write(0); }

 private:
  MotorPins pins_;
};

