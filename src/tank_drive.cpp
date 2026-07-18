#include "tank_drive.h"

void TankDrive::begin() {
  pinMode(config_.standbyPin, OUTPUT);
  digitalWrite(config_.standbyPin, LOW);
  left_.begin(config_.pwmFrequencyHz, config_.pwmResolutionBits);
  right_.begin(config_.pwmFrequencyHz, config_.pwmResolutionBits);
  digitalWrite(config_.standbyPin, HIGH);
}

void TankDrive::command(Motion motion) {
  target_ = speedsForMotion(motion, config_.speeds);
}

void TankDrive::update(uint32_t nowMs) {
  if (nowMs - lastRampMs_ < config_.rampIntervalMs) return;
  lastRampMs_ = nowMs;
  current_.left = rampToward(current_.left, target_.left, config_.rampStep);
  current_.right = rampToward(current_.right, target_.right, config_.rampStep);
  left_.write(current_.left);
  right_.write(current_.right);
}

void TankDrive::emergencyStop() {
  target_ = current_ = {0, 0};
  left_.stop();
  right_.stop();
}

