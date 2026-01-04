#include "socket/udp_socket.h"

#include "socket/socket_address.h"
#include "socket/socket_util.h"

GameNet::UDPSocket::~UDPSocket() {
#if _WIN32
  closesocket(mSocket);
#else
  close(mSocket);
#endif
}

GameNet::UDPSocketPtr GameNet::UDPSocket::Create(SocketAddressFamily family) {
  SOCKET s = socket(family, SOCK_DGRAM, IPPROTO_UDP);
  if (s == INVALID_SOCKET) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to create a socket\n",
                __FUNCTION__);
    return nullptr;
  }

  return UDPSocketPtr(new UDPSocket(s));
}

int GameNet::UDPSocket::Bind(const SocketAddress& bindAddr) {
  int err = bind(mSocket, &bindAddr.mSockAddr, bindAddr.GetSockAddrSize());
  if (err != 0) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to bind\n", __FUNCTION__);
    return SocketUtil::GetLastError();
  }

  return NO_ERROR;
}

int GameNet::UDPSocket::SendTo(const void* buf, int len, const SocketAddress& toAddr) {
  int byteSent = sendto(mSocket, static_cast<const char*>(buf), len, 0,
                        &toAddr.mSockAddr, toAddr.GetSockAddrSize());
  if (byteSent < 0) {
    // Return error code as negative number.
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to send\n", __FUNCTION__);
    return -SocketUtil::GetLastError();
  }

  return byteSent;
}

int GameNet::UDPSocket::ReceiveFrom(void* buf, int maxLen, SocketAddress& fromAddress) {
  socklen_t fromAddrLen = fromAddress.GetSockAddrSize();

  int byteRecv = recvfrom(mSocket, static_cast<char*>(buf), maxLen, 0,
                          &fromAddress.mSockAddr, &fromAddrLen);
  if (byteRecv < 0) {
    int err = SocketUtil::GetLastError();
#if _WIN32
    if (err == WSAEWOULDBLOCK) {
      return 0;
    } else if (err == WSAECONNRESET) {
      // This can happen if a client closed and we haven't DC'd yet.
      // This is the ICMP message being sent back saying the port on that
      // computer is closed
      Logger::Log(LOG_SEVERITY_INFO, "%s: Connection reset from %s\n", __FUNCTION__,
                  fromAddress.ToString().c_str());
      return -WSAECONNRESET;
    }
#endif
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to receive\n", __FUNCTION__);
    return -SocketUtil::GetLastError();
  }

  return byteRecv;
}

int GameNet::UDPSocket::SetNonBlockingMode(bool nonBlocking) {
#if _WIN32
  unsigned long arg = nonBlocking ? 1ul : 0ul;
  int res = ioctlsocket(mSocket, FIONBIO, &arg);
#else
  int flags = fcntl(mSocket, F_GETFL, 0);
  flags = nonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
  int res = fcntl(mSocket, F_SETFL, flags);
#endif

  if (res == SOCKET_ERROR) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s: failed to set non-blocking mode\n",
                __FUNCTION__);
    return SocketUtil::GetLastError();
  }

  return NO_ERROR;
}
