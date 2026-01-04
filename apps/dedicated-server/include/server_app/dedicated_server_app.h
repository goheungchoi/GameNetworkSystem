#pragma once

#include "gamenet_app/app_base.h"

namespace GameNet {

class DedicatedServerApp final : public AppBase {
 public:
  DedicatedServerApp() = default;

 protected:
  int Init(int argc, char** argv) override;
  int Run() override;
  void Shutdown() override;
};

}  // namespace GameNet
