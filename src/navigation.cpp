#include "navigation.h"

Navigator::Navigator(NavConfig config) : config_(config) {}

void Navigator::reset(uint32_t nowMs) {
  phase_ = NavPhase::Cruising;
  phaseStartedMs_ = nowMs;
  invalidReadings_ = 0;
  turnLeftNext_ = true;
}

void Navigator::enter(NavPhase phase, uint32_t nowMs) {
  phase_ = phase;
  phaseStartedMs_ = nowMs;
}

Motion Navigator::motionForPhase() const {
  switch (phase_) {
    case NavPhase::Cruising: return Motion::Forward;
    case NavPhase::Reversing: return Motion::Reverse;
    case NavPhase::Turning: return turnLeftNext_ ? Motion::TurnLeft : Motion::TurnRight;
    case NavPhase::Recovering: return Motion::Forward;
    case NavPhase::Pause:
    case NavPhase::Fault:
    default: return Motion::Stop;
  }
}

NavOutput Navigator::update(float distanceCm, bool readingValid, uint32_t nowMs) {
  const NavPhase previous = phase_;

  if (readingValid) {
    invalidReadings_ = 0;
    if (phase_ == NavPhase::Fault) enter(NavPhase::Pause, nowMs);
  } else if (invalidReadings_ < 255) {
    ++invalidReadings_;
  }

  if (invalidReadings_ >= config_.maxInvalidReadings) {
    enter(NavPhase::Fault, nowMs);
  } else {
    const uint32_t elapsed = nowMs - phaseStartedMs_;  // wrap-safe
    switch (phase_) {
      case NavPhase::Cruising:
        if (readingValid && distanceCm <= config_.stopDistanceCm) enter(NavPhase::Pause, nowMs);
        break;
      case NavPhase::Pause:
        if (elapsed >= config_.stopPauseMs) enter(NavPhase::Reversing, nowMs);
        break;
      case NavPhase::Reversing:
        if (elapsed >= config_.reverseTimeMs) enter(NavPhase::Turning, nowMs);
        break;
      case NavPhase::Turning:
        if (elapsed >= config_.turnTimeMs) {
          turnLeftNext_ = !turnLeftNext_;
          enter(NavPhase::Recovering, nowMs);
        }
        break;
      case NavPhase::Recovering:
        if (readingValid && distanceCm <= config_.stopDistanceCm) enter(NavPhase::Pause, nowMs);
        else if (elapsed >= config_.recoveryTimeMs && readingValid && distanceCm >= config_.clearDistanceCm)
          enter(NavPhase::Cruising, nowMs);
        break;
      case NavPhase::Fault:
        break;
    }
  }

  return {motionForPhase(), phase_, phase_ != previous};
}

const char* motionName(Motion motion) {
  switch (motion) {
    case Motion::Forward: return "forward";
    case Motion::Reverse: return "reverse";
    case Motion::TurnLeft: return "left";
    case Motion::TurnRight: return "right";
    default: return "stop";
  }
}

const char* phaseName(NavPhase phase) {
  switch (phase) {
    case NavPhase::Cruising: return "cruising";
    case NavPhase::Pause: return "pause";
    case NavPhase::Reversing: return "reversing";
    case NavPhase::Turning: return "turning";
    case NavPhase::Recovering: return "recovering";
    default: return "sensor fault";
  }
}

