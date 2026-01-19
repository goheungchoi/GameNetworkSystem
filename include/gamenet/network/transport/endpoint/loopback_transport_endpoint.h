#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <span>

#include "endpoint/network_endpoint_interface.h"

namespace GameNet {

/**
 * @brief In-process transport endpoint.
 *
 * It wires up two LoopbackTransportEndpoints.
 * This is useful for listen-server mode, Play-In_Editor, unit tests, and etc.
 *
 * (multiple endpoints need to be wired up to make a listen server)
 */
class LoopbackTransportEndpoint final : public INetworkTransportEndpoint {
 public:
  struct ConnectedEndpoints {
    std::unique_ptr<LoopbackTransportEndpoint> clientEndpoint;
    std::unique_ptr<LoopbackTransportEndpoint> serverEndpoint;
  };

  static ConnectedEndpoints CreateConnectedEndpoints(
      const SocketAddress& clientAddress, const SocketAddress& serverAddress);

  LoopbackTransportEndpoint(const LoopbackTransportEndpoint&) = delete;
  LoopbackTransportEndpoint& operator=(const LoopbackTransportEndpoint&) =
      delete;

  bool SendPacket(const SocketAddress& dest,
                  std::span<const uint8_t> payload) override;

  bool PollPacket(NetworkReceivedPacket& packet) override;

  SocketAddress GetLocalSocketAddress() const override { return mLocalAddress; }

 private:
  enum class EndpointSide { ClientSide, ServerSide };

  struct SharedMemoryBuffer {
    std::mutex m;
    std::deque<NetworkReceivedPacket> packetsForClient;
    std::deque<NetworkReceivedPacket> packetsForServer;
  };

  LoopbackTransportEndpoint(std::shared_ptr<SharedMemoryBuffer> sharedBuffer,
                            EndpointSide side,
                            const SocketAddress& localAddress,
                            const SocketAddress& peerAddress);

  std::deque<NetworkReceivedPacket>& GetIncomingQueueToRead();
  std::deque<NetworkReceivedPacket>& GetPeerIncomingQueueToWrite();

  std::shared_ptr<SharedMemoryBuffer> mSharedBuffer;
  EndpointSide mSide;
  SocketAddress mLocalAddress;
  SocketAddress mPeerAddress;
};

}  // namespace GameNet
