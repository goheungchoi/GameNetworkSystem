#pragma once

#include "gamenet_app/app/glfw_vulkan_app.h"

namespace GameNet {

class GameApp final : public GLFWApp {
 public:
  GameApp();

 protected:
  int OnInit(int argc, char** argv) override;
  int OnTick() override;
  void OnShutdown() override;

  void OnResize(int width, int height) override;
  void OnKey(int key, int scancode, int action, int mods) override;
  void OnMouseButton(int button, int action, int mods) override;
  void OnMouseMove(double xpos, double ypos) override;

 private:
  std::unique_ptr<class GameSession> _gameSession; 
};

}  // namespace GameNet
