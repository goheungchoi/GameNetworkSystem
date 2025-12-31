#pragma once
#include "socket_address.h"
#include "socket_includes.h"

namespace GameNet {
  
class SocketAddressFactory {
 public:
  static SocketAddressOpt CreateIPv4FromString(const std::string& str);
};

}  // namespace GameNet
