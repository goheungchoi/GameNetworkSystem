#include "endpoint/udp_transport_endpoint.h"

std::unique_ptr<GameNet::UDPTransportEndpoint>
GameNet::UDPTransportEndpoint::Create(const SocketAddress& address,
                                      int maximumPacketSize) {
  if (maximumPacketSize <= 0) {
    Logger::Log(LOG_SEVERITY_ERROR,
                "%s error: maximumPacketSize must be positive\n", __FUNCTION__);
    return nullptr;
  }

  UDPSocketPtr socket = UDPSocket::Create(INET);
  if (!socket) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: failed to create UDP socket\n");
    return nullptr;
  }

  int err = socket->Bind(address);
  if (err != NO_ERROR) {
    Logger::Log(LOG_SEVERITY_ERROR,
                "%s error: failed to bind UDP socket (address=%s, error=%d)\n",
                __FUNCTION__, address.ToString().c_str(), err);
    return nullptr;
  }

  err = socket->SetNonBlockingMode(true);
  if (err != NO_ERROR) {
    Logger::Log(LOG_SEVERITY_ERROR,
                "%s error: failed to set non-blocking mode (error=%d)\n",
                __FUNCTION__, err);
    return nullptr;
  }

  return std::unique_ptr<UDPTransportEndpoint>(
      new UDPTransportEndpoint(std::move(socket), address, maximumPacketSize));
}

GameNet::UDPTransportEndpoint::UDPTransportEndpoint(
    UDPSocketPtr socket, const SocketAddress& address, int maximumPacketSize)
    : mSocket(std::move(socket)),
      mAddress{address},
      mReceivedBuffer(static_cast<size_t>(maximumPacketSize)) {}

bool GameNet::UDPTransportEndpoint::SendPacket(
    const SocketAddress& dest, std::span<const uint8_t> payload) {
  if (!mSocket) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: UDP socket is null\n",
                __FUNCTION__);
    return false;
  }

  if (payload.empty()) {
    // No need to process empty payload.
    return true;
  }

  // UDPSocket expects a void pointer and length as int.
  const int payloadSize = static_cast<int>(payload.size());
  const int bytesSent = mSocket->SendTo(payload.data(), payloadSize, dest);
  if (bytesSent < 0) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: sendto failed (error=%d)\n",
                __FUNCTION__, -bytesSent);
    return false;
  }

  if (bytesSent != payloadSize) {
    // UDP should either send all or fail; partial send is unexpected.
    Logger::Log(LOG_SEVERITY_WARNING,
                "%s warning: partial sent (sent=%d, expected=%d)\n", bytesSent,
                payloadSize);
  }

  return true;
}

bool GameNet::UDPTransportEndpoint::PollPacket(NetworkReceivedPacket& outPacket) {
  if (!mSocket) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: UDP socket is null\n",
                __FUNCTION__);
    return false;
  }

  SocketAddress sourceAddress;
  const int bytesReceived = mSocket->ReceiveFrom(
      mReceivedBuffer.data(), static_cast<int>(mReceivedBuffer.size()),
      sourceAddress);
  if (bytesReceived <= 0) {
    // 0 indicates no data (would-block), negative indicates error.
    if (bytesReceived < 0) {
      Logger::Log(LOG_SEVERITY_ERROR, "%s error: recvfrom failed (error=%d)\n",
                  -bytesReceived);
    }
    return false;
  }

  outPacket.sourceAddress = sourceAddress;
  outPacket.payload.resize(static_cast<size_t>(bytesReceived));
  std::memcpy(outPacket.payload.data(), mReceivedBuffer.data(),
              static_cast<size_t>(bytesReceived));

  return true;
}
