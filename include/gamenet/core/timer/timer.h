#pragma once

#include <algorithm>
#include <chrono>

namespace GameNet {

class FrameClock {
  using clock = std::chrono::steady_clock;
  clock::time_point prev = clock::now();

 public:
  double maxDelta = 0.25;  // seconds
  double timeScale = 1.0;

  double Tick() {
    auto now = clock::now();
    std::chrono::duration<double> d = now - prev;
    prev = now;

    double dt = d.count();
    dt = std::clamp<double>(dt, 0.0, maxDelta);
    return dt * timeScale;
  }
};

class FixedStepper {
 public:
  double fixedDelta = 1.0 / 60.0;
  double accumulator = 0.0;

  void AddTime(double dt) { accumulator += dt; }

  bool Step() {
    if (accumulator >= fixedDelta) {
      accumulator -= fixedDelta;
      return true;
    }
    return false;
  }

  double Alpha() const {
    return fixedDelta > 0.0 ? (accumulator / fixedDelta) : 0.0;
  }
};

}  // namespace GameNet
