#pragma once
#include <atomic>
#include <memory>

namespace GameNet {
class AppBase {
 public:
  AppBase() = default;
  virtual ~AppBase() = default;

  int Exec(int argc, char** argv);

 protected:
  virtual int Init(int argc, char** argv) = 0;
  virtual int Run() = 0;
  virtual void Shutdown() = 0;
};

}  // namespace GameNet
