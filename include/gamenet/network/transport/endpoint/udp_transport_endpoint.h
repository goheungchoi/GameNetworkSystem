#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <vector>

#include "endpoint/network_endpoint_interface.h"
#include "socket/udp_socket.h"

namespace GameNet {

/**
 * @brief A UDP-backed transport endpoint.
 *
 * It only sends and receives raw datagrams.
 * Higher-level modules should handle framing, reliability, and datagram
 * semantics.
 *
 */
class UDPTransportEndpoint final : public INetworkTransportEndpoint {
 public:
  static constexpr int kDefaultMaximumPacketSize = 1200;  // in bytes.

  /**
   * @brief Create a udp socket and bind it to the address.
   * Set it to a non-blocking mode.
   * 
   * @param address the local address foe the socket to be bound
   * @param maximumPacketSize 
   * @return std::unique_ptr<UDPTransportEndpoint> 
   */
  static std::unique_ptr<UDPTransportEndpoint> Create(
      const SocketAddress& address,
      int maximumPacketSize = kDefaultMaximumPacketSize);

  UDPTransportEndpoint(const UDPTransportEndpoint&) = delete;
  UDPTransportEndpoint& operator=(const UDPTransportEndpoint&) = delete;

  bool SendPacket(const SocketAddress& dest,
                  std::span<const uint8_t> payload) override;

  bool PollPacket(NetworkReceivedPacket& outPacket) override;

  SocketAddress GetLocalSocketAddress() const override {
    return mAddress;
  }

 private:
  explicit UDPTransportEndpoint(UDPSocketPtr socket,
                                const SocketAddress& address,
                                int maximumPacketSize);

  UDPSocketPtr mSocket;
  SocketAddress mAddress;
  std::vector<uint8_t> mReceivedBuffer;
};

}  // namespace GameNet