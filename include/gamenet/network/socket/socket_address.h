#pragma once
#include "socket_includes.h"

namespace GameNet {

class SocketAddress {
 public:
  SocketAddress() {
    GetAsSockAddrIn()->sin_family = AF_INET;
    GetIP4Ref() = INADDR_ANY;
    GetAsSockAddrIn()->sin_port = 0;
  }

  SocketAddress(uint32_t address, uint16_t port) {
    GetAsSockAddrIn()->sin_family = AF_INET;  // IPv4
    GetIP4Ref() = htonl(address);
    GetAsSockAddrIn()->sin_port = htons(port);
  }

  SocketAddress(const sockaddr& sockAddr) {
    memcpy(&mSockAddr, &sockAddr, sizeof(sockaddr));
  }

  bool operator==(const SocketAddress& other) const {
    return mSockAddr.sa_family == AF_INET &&
           GetAsSockAddrIn()->sin_port == other.GetAsSockAddrIn()->sin_port &&
           GetIP4Ref() == other.GetIP4Ref();
  }

  size_t GetHash() const {
    return std::hash<std::remove_cvref_t<decltype(GetIP4Ref())>>()(
               GetIP4Ref()) |
           std::hash<
               std::remove_cvref_t<decltype(GetAsSockAddrIn()->sin_port)>>()(
               GetAsSockAddrIn()->sin_port)
               << 13 |
           std::hash<std::remove_cvref_t<decltype(mSockAddr.sa_family)>>()(
               mSockAddr.sa_family);
  }

  socklen_t GetSockAddrSize() const {
    return static_cast<socklen_t>(sizeof(sockaddr));
  }

  std::string ToString() const;

 private:
  friend class UDPSocket;
  friend class TCPSocket;

  sockaddr mSockAddr;

#if _WIN32
  uint32_t& GetIP4Ref() {
    return *reinterpret_cast<uint32_t*>(
        &GetAsSockAddrIn()->sin_addr.S_un.S_addr);
  }
  const uint32_t& GetIP4Ref() const {
    return *reinterpret_cast<const uint32_t*>(
        &GetAsSockAddrIn()->sin_addr.S_un.S_addr);
  }
#else
  uint32_t& GetIP4Ref() { return GetAsSockAddrIn()->sin_addr.s_addr; }
  const uint32_t& GetIP4Ref() const {
    return GetAsSockAddrIn()->sin_addr.s_addr;
  }
#endif

  sockaddr_in* GetAsSockAddrIn() {
    return reinterpret_cast<sockaddr_in*>(&mSockAddr);
  }
  const sockaddr_in* GetAsSockAddrIn() const {
    return reinterpret_cast<const sockaddr_in*>(&mSockAddr);
  }
};

using SocketAddressOpt = std::optional<SocketAddress>;
using SocketAddressRefOpt =
    std::optional<std::reference_wrapper<SocketAddress>>;

}  // namespace GameNet

namespace std {
template <>
struct hash<GameNet::SocketAddress> {
  size_t operator()(const GameNet::SocketAddress& addr) const {
    return addr.GetHash();
  }
};
}  // namespace std
