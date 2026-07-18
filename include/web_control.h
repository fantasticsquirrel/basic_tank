#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include "drive_model.h"
#include "navigation.h"

enum class ControlMode { Auto, Manual };

struct ControlTelemetry {
  float distanceCm = 0;
  bool rangeValid = false;
  Motion motion = Motion::Stop;
  const char* phase = "starting";
};

class WebControl {
 public:
  WebControl(const char* ssid, const char* password, const char* token,
             uint16_t deadmanMs);
  void begin();
  void update();
  ControlMode mode() const { return mode_; }
  bool commandFresh(uint32_t nowMs) const;
  WheelSpeeds manualSpeeds() const { return manualSpeeds_; }
  void setTelemetry(const ControlTelemetry& telemetry) { telemetry_ = telemetry; }
  const char* ipAddress() const;

 private:
  bool authorized();
  void addCors();
  void handleOptions();
  void handleMode();
  void handleControl();
  void handleStatus();
  void sendJson(int status, const String& body);

  const char* ssid_;
  const char* password_;
  const char* token_;
  uint16_t deadmanMs_;
  WebServer server_{80};
  ControlMode mode_ = ControlMode::Auto;
  WheelSpeeds manualSpeeds_{0, 0};
  uint32_t lastCommandMs_ = 0;
  uint32_t lastSequence_ = 0;
  bool hasCommand_ = false;
  ControlTelemetry telemetry_;
  mutable String ipCache_;
};

