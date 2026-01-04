#pragma once

#include <string>

#include "app_base.h"

struct GLFWwindow;

namespace GameNet {

class GLFWApp : public AppBase {
  std::string _windowTitle{};
  GLFWwindow* _window{nullptr};

 protected:
  int Init(int argc, char** argv) final;
  int Run() final;
  void Shutdown() final;

  virtual int OnInit(int argc, char** argv) = 0;
  virtual int OnTick() = 0;
  virtual void OnShutdown() = 0;

  GLFWwindow* GetWindow() { return _window; }
  const GLFWwindow* GetWindow() const { return _window; };
  void SetWindowTitle(const std::string& windowTitle);
  const std::string& GetWindowTitle() const;
  
  void RequestWindowClose();

  virtual void OnResize(int width, int height) = 0;
  virtual void OnKey(int key, int scancode, int action, int mods) = 0;
  virtual void OnMouseButton(int button, int action, int mods) = 0;
  virtual void OnMouseMove(double xpos, double ypos) = 0;

 private:
  static void ResizeCallback(GLFWwindow* win, int width, int height);
  static void KeyEventCallback(GLFWwindow* win, int key, int scancode,
                               int action, int mods);
  static void MouseButtonCallback(GLFWwindow* win, int button, int action,
                                  int mods);
  static void MousePositionCallback(GLFWwindow* win, double xpos, double ypos);
};

}  // namespace GameNet
