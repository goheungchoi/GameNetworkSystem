#include "endpoint/loopback_transport_endpoint.h"

GameNet::LoopbackTransportEndpoint::ConnectedEndpoints
GameNet::LoopbackTransportEndpoint::CreateConnectedEndpoints(
    const SocketAddress& clientAddress, const SocketAddress& serverAddress) {
  auto sharedBuffer = std::make_shared<SharedMemoryBuffer>();

  ConnectedEndpoints connectedEndpoints;
  connectedEndpoints.clientEndpoint =
      std::unique_ptr<LoopbackTransportEndpoint>(
          new LoopbackTransportEndpoint(sharedBuffer, EndpointSide::ClientSide,
                                        clientAddress, serverAddress));

  connectedEndpoints.serverEndpoint =
      std::unique_ptr<LoopbackTransportEndpoint>(
          new LoopbackTransportEndpoint(sharedBuffer, EndpointSide::ServerSide,
                                        serverAddress, clientAddress));

  return connectedEndpoints;
}

bool GameNet::LoopbackTransportEndpoint::SendPacket(
    const SocketAddress& /*unused*/, std::span<const uint8_t> payload) {
  if (!mSharedBuffer) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: shared memory buffer is null\n");
    return false;
  }

  NetworkReceivedPacket packetForPeer;
  packetForPeer.sourceAddress = mLocalAddress;
  packetForPeer.payload.assign(payload.begin(), payload.end());

  {
    std::scoped_lock lock(mSharedBuffer->m);
    GetPeerIncomingQueueToWrite().push_back(std::move(packetForPeer));
  }

  return true;
}

bool GameNet::LoopbackTransportEndpoint::PollPacket(
    NetworkReceivedPacket& outPacket) {
  if (!mSharedBuffer) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: shared memory buffer is null\n");
    return false;
  }

  std::scoped_lock lock(mSharedBuffer->m);
  std::deque<NetworkReceivedPacket>& incomingQueue = GetIncomingQueueToRead();
  if (incomingQueue.empty()) {
    return false;
  }

  outPacket = std::move(incomingQueue.front());
  incomingQueue.pop_front();
  return true;
}

std::deque<GameNet::NetworkReceivedPacket>&
GameNet::LoopbackTransportEndpoint::GetIncomingQueueToRead() {
  return (mSide == EndpointSide::ClientSide) ? mSharedBuffer->packetsForClient
                                             : mSharedBuffer->packetsForServer;
}

std::deque<GameNet::NetworkReceivedPacket>&
GameNet::LoopbackTransportEndpoint::GetPeerIncomingQueueToWrite() {
  return (mSide == EndpointSide::ClientSide) ? mSharedBuffer->packetsForServer
                                             : mSharedBuffer->packetsForClient;
}
