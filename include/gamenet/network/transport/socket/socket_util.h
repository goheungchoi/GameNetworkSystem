#pragma once
#include "socket_includes.h"

namespace GameNet {
  
class SocketUtil {
 public:
  static bool StaticInit();
  static void StaticCleanUp();

  static int GetLastError();
};

}  // namespace GameNet
