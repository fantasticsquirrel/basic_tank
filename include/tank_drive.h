#pragma once

#include <Arduino.h>
#include "drive_model.h"
#include "motor.h"

struct TankDriveConfig {
  uint8_t standbyPin;
  uint16_t pwmFrequencyHz;
  uint8_t pwmResolutionBits;
  int rampStep;
  uint16_t rampIntervalMs;
  DriveSpeeds speeds;
};

class TankDrive {
 public:
  TankDrive(Motor& left, Motor& right, TankDriveConfig config)
      : left_(left), right_(right), config_(config) {}
  void begin();
  void command(Motion motion);
  void commandWheels(WheelSpeeds speeds);
  void update(uint32_t nowMs);
  void emergencyStop();

 private:
  Motor& left_;
  Motor& right_;
  TankDriveConfig config_;
  WheelSpeeds current_{0, 0};
  WheelSpeeds target_{0, 0};
  uint32_t lastRampMs_ = 0;
};
