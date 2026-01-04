#pragma once

#include <memory>

#include "core/timer/timer.h"

struct GLFWwindow;

namespace GameNet {

enum class SessionMode {
  Listen,      // Listen server + client
  ClientOnly,  // Local client connected to a remote server.
};

class GameSession {
  SessionMode mode;

  FrameClock frame;
  FixedStepper stepper;
  uint32_t tickId = 0;

  std::unique_ptr<class ClientRuntime> client;
  std::unique_ptr<class ServerRuntime> server;  // null unless listen
  
  // LoopbackTransport or UdpTransport
  std::unique_ptr<class ITransport> transport;  

 public:
  GameSession();

  int Init();
  int Frame();
  void Shutdown();
};

}  // namespace GameNet
