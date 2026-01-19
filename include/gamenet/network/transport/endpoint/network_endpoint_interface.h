#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "socket/socket_address.h"

namespace GameNet {

// Transport-level datagram.
struct NetworkReceivedPacket {
  SocketAddress sourceAddress{};
  std::vector<uint8_t> payload;

  void Clear() { payload.clear(); }
};

class INetworkTransportEndpoint {
 public:
  virtual ~INetworkTransportEndpoint() = default;

  /**
   * @brief Send a datagram to the destination address.
   * 
   * @param dest 
   * @param payload 
   * @return true if the endpoint accepted the send request.
   * @return false 
   */
  virtual bool SendPacket(const SocketAddress& dest,
                          std::span<const uint8_t> payload) = 0;

  /**
   * @brief Polls a single received datagram.
   * 
   * @return true if a datagram was written into the packet.
   * @return false 
   */
  virtual bool PollPacket(NetworkReceivedPacket& outPacket) = 0;

  virtual SocketAddress GetLocalSocketAddress() const = 0;
};

}  // namespace GameNet
