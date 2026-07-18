#include "robot_app.h"

void RobotApp::begin() {
  drive_.begin();
  range_.begin();
  web_.begin();
  navigator_.reset(millis());
  Serial.println("basic_tank ready; lift treads for first test");
}

void RobotApp::update(uint32_t nowMs) {
  web_.update();
  if (web_.mode() == ControlMode::Manual) {
    drive_.commandWheels(web_.commandFresh(nowMs) ? web_.manualSpeeds() : WheelSpeeds{0, 0});
    drive_.update(nowMs);
    web_.setTelemetry({0, false, Motion::Stop,
                       web_.commandFresh(nowMs) ? "manual" : "manual timeout"});
    return;
  }
  drive_.update(nowMs);
  if (nowMs - lastSenseMs_ < sensorIntervalMs_) return;
  lastSenseMs_ = nowMs;

  const RangeReading reading = range_.read();
  const NavOutput output = navigator_.update(reading.centimeters, reading.valid, millis());
  drive_.command(output.motion);
  web_.setTelemetry({reading.centimeters, reading.valid, output.motion, phaseName(output.phase)});
  Serial.printf("range=%s", reading.valid ? "" : "invalid");
  if (reading.valid) Serial.printf("%.1fcm", reading.centimeters);
  Serial.printf(" phase=%s motion=%s%s\n", phaseName(output.phase),
                motionName(output.motion), output.changed ? " *" : "");
}
