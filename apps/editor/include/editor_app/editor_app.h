#pragma once

#include "gamenet_app/app_base.h"

namespace GameNet {

class EditorApp final : public AppBase {
 public:
  EditorApp() = default;

 protected:
  int Init(int argc, char** argv) override;
  int Run() override;
  void Shutdown() override;

 private:
  AppBase* app = nullptr;
  FrameClock frameClock;
  FixedStepper stepper;

  struct {
    bool active = false;
    bool paused = false;
    bool stepOnceRequested = false;
    double timeScale = 1.0;
    uint32_t tickId = 0;
    // pointers/owners to ServerInstance, ClientInstance, LoopbackTransport,
    // etc.
  } session;

  void StepRuntimeOnce() {
    ++session.tickId;

    // Typically in this order for PIE:
    // 1) Client: sample input for tickId, buffer, send (loopback)
    // 2) Server: receive inputs, simulate one tick
    // 3) Server: send snapshot occasionally
    // 4) Client: receive snapshot, reconcile, simulate predicted tick
  }
};

}  // namespace GameNet
