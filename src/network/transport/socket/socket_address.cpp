#include "socket/socket_address.h"

std::string GameNet::SocketAddress::ToString() const {
  const sockaddr_in* s = GetAsSockAddrIn();
  char destBuff[128];

#if _WIN32
  // InetNtop is the Windows variant of inet_ntop
  if (InetNtopA(s->sin_family, const_cast<in_addr*>(&s->sin_addr), destBuff,
                static_cast<DWORD>(sizeof(destBuff))) == nullptr) {
    return {};
  }
#else
  if (inet_ntop(s->sin_family, &s->sin_addr, destBuff, sizeof(destBuff)) ==
      nullptr) {
    return {};
  }
#endif
  return std::format("{}:{}", destBuff, ntohs(s->sin_port));
}
