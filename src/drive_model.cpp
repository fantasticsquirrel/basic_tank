#include "drive_model.h"

#include <algorithm>

WheelSpeeds speedsForMotion(Motion motion, const DriveSpeeds& speeds) {
  switch (motion) {
    case Motion::Forward: return {speeds.cruise, speeds.cruise};
    case Motion::Reverse: return {-speeds.reverse, -speeds.reverse};
    case Motion::TurnLeft: return {-speeds.turn, speeds.turn};
    case Motion::TurnRight: return {speeds.turn, -speeds.turn};
    default: return {0, 0};
  }
}

int rampToward(int current, int target, int step) {
  if (step <= 0) return target;
  if (current < target) return std::min(current + step, target);
  if (current > target) return std::max(current - step, target);
  return current;
}

