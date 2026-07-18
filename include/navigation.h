#pragma once

#include <cstdint>

enum class Motion { Stop, Forward, Reverse, TurnLeft, TurnRight };
enum class NavPhase { Cruising, Pause, Reversing, Turning, Recovering, Fault };

struct NavConfig {
  float stopDistanceCm;
  float clearDistanceCm;
  uint8_t maxInvalidReadings;
  uint32_t stopPauseMs;
  uint32_t reverseTimeMs;
  uint32_t turnTimeMs;
  uint32_t recoveryTimeMs;
};

struct NavOutput {
  Motion motion;
  NavPhase phase;
  bool changed;
};

class Navigator {
 public:
  explicit Navigator(NavConfig config);
  NavOutput update(float distanceCm, bool readingValid, uint32_t nowMs);
  void reset(uint32_t nowMs = 0);
  NavPhase phase() const { return phase_; }

 private:
  void enter(NavPhase phase, uint32_t nowMs);
  Motion motionForPhase() const;

  NavConfig config_;
  NavPhase phase_ = NavPhase::Cruising;
  uint32_t phaseStartedMs_ = 0;
  uint8_t invalidReadings_ = 0;
  bool turnLeftNext_ = true;
};

const char* motionName(Motion motion);
const char* phaseName(NavPhase phase);

