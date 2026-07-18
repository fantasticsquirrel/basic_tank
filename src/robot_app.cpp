#include "robot_app.h"

void RobotApp::begin() {
  drive_.begin();
  range_.begin();
  navigator_.reset(millis());
  Serial.println("basic_tank ready; lift treads for first test");
}

void RobotApp::update(uint32_t nowMs) {
  drive_.update(nowMs);
  if (nowMs - lastSenseMs_ < sensorIntervalMs_) return;
  lastSenseMs_ = nowMs;

  const RangeReading reading = range_.read();
  const NavOutput output = navigator_.update(reading.centimeters, reading.valid, millis());
  drive_.command(output.motion);
  Serial.printf("range=%s", reading.valid ? "" : "invalid");
  if (reading.valid) Serial.printf("%.1fcm", reading.centimeters);
  Serial.printf(" phase=%s motion=%s%s\n", phaseName(output.phase),
                motionName(output.motion), output.changed ? " *" : "");
}

