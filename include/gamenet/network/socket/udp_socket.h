#pragma once
#include "socket_includes.h"

namespace GameNet {
  
class SocketAddress;

using UDPSocketPtr = std::unique_ptr<class UDPSocket>;

class UDPSocket {
 public:
  ~UDPSocket();

  static UDPSocketPtr Create(SocketAddressFamily family);

  int Bind(const SocketAddress& bindAddr);
  int SendTo(const void* buf, int len, const SocketAddress& toAddr);
  int ReceiveFrom(void* buf, int maxLen, SocketAddress& fromAddress);

  int SetNonBlockingMode(bool nonBlocking);

 private:
  UDPSocket(SOCKET socket) : mSocket(socket) {}
  SOCKET mSocket;
};

}  // namespace GameNet
