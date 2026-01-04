#include "socket/tcp_socket.h"

#include "socket/socket_address.h"
#include "socket/socket_util.h"

GameNet::TCPSocket::~TCPSocket() {
#if _WIN32
  closesocket(mSocket);
#else
  close(mSocket);
#endif
}

GameNet::TCPSocketPtr GameNet::TCPSocket::Create(SocketAddressFamily family) {
  SOCKET s = socket(family, SOCK_STREAM, IPPROTO_TCP);
  if (s == INVALID_SOCKET) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to create TCP socket\n",
                __FUNCTION__);
    return nullptr;
  }

  return TCPSocketPtr(new TCPSocket(s));
}

int GameNet::TCPSocket::Connect(const SocketAddress& toAddr) {
  int err = connect(mSocket, &toAddr.mSockAddr, toAddr.GetSockAddrSize());
  if (err < 0) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to connect\n", __FUNCTION__);
    return -SocketUtil::GetLastError();
  }

  return NO_ERROR;
}

int GameNet::TCPSocket::Bind(const SocketAddress& bindAddr) {
  int err = bind(mSocket, &bindAddr.mSockAddr, bindAddr.GetSockAddrSize());
  if (err != 0) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to bind\n", __FUNCTION__);
    return SocketUtil::GetLastError();
  }

  return NO_ERROR;
}

int GameNet::TCPSocket::Listen(int backlog) {
  int err = listen(mSocket, backlog);
  if (err < 0) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to listen\n", __FUNCTION__);
    return -SocketUtil::GetLastError();
  }

  return NO_ERROR;
}

GameNet::TCPSocketPtr GameNet::TCPSocket::Accept(SocketAddress& fromAddr) {
  socklen_t socklen = fromAddr.GetSockAddrSize();
  SOCKET newSocket = accept(mSocket, &fromAddr.mSockAddr, &socklen);
  if (newSocket == INVALID_SOCKET) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to accept\n", __FUNCTION__);
    return nullptr;
  }

  return TCPSocketPtr(new TCPSocket(newSocket));
}

int GameNet::TCPSocket::Send(const void* buf, int len) {
  int byteSent = send(mSocket, static_cast<const char*>(buf), len, 0);
  if (byteSent < 0) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to send\n", __FUNCTION__);
    return -SocketUtil::GetLastError();
  }

  return byteSent;
}

int GameNet::TCPSocket::Receive(void* buf, int maxLen) {
  int byteRecv = recv(mSocket, static_cast<char*>(buf), maxLen, 0);

  if (byteRecv < 0) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to receive\n", __FUNCTION__);
    return -SocketUtil::GetLastError();
  }

  return byteRecv;
}
