#pragma once

class ISession {
 public:
  virtual int Init() = 0;
  virtual int Advance() = 0;
  virtual void Shutdown() = 0;

  virtual void ProcessResize(int width, int height) = 0;
  virtual void ProcessKeyEvents(int key, int scancode, int action, int mods) = 0;
  virtual void ProcessMouseButtonEvents(int button, int action, int mods) = 0;
  virtual void ProcessMousePositionEvents(double xpos, double ypos) = 0;
};
