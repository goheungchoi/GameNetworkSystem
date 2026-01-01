#pragma once
#include "socket_includes.h"

namespace GameNet {

class SocketAddress;

using TCPSocketPtr = std::unique_ptr<class TCPSocket>;

class TCPSocket {
 public:
  ~TCPSocket();

  static TCPSocketPtr Create(SocketAddressFamily family);

  int Connect(const SocketAddress& toAddr);
  int Bind(const SocketAddress& bindAddr);
  int Listen(int backlog = 32);
  TCPSocketPtr Accept(SocketAddress& fromAddr);
  int Send(const void* buf, int len);
  int Receive(void* buf, int maxLen);

 private:
  TCPSocket(SOCKET socket) : mSocket(socket) {}
  SOCKET mSocket;
};

}  // namespace GameNet
