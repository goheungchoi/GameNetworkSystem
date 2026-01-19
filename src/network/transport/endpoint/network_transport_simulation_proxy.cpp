#include "endpoint/network_transport_simulation_proxy.h"

GameNet::NetworkTransportSimulationProxy::NetworkTransportSimulationProxy(
    std::unique_ptr<INetworkTransportEndpoint> endpoint,
    NetworkTransportSimulationSettings settings)
    : mEndpoint(std::move(endpoint)),
      mSettings(settings),
      mProbabilityDistribution(0.0, 1.0),
      mJitterDistribution(-settings.jitter, settings.jitter) {
  std::random_device randomDevice;
  mRandomNumberGenerator = std::mt19937(randomDevice());

  if (!mEndpoint) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: endpoint is null\n",
                __FUNCTION__);
  }

  // Clamp settings to sensible ranges.
  mSettings.packetLossProbability =
      std::clamp(mSettings.packetLossProbability, 0.0, 1.0);

  mSettings.baseLatency = std::max(0.0, mSettings.baseLatency);

  mSettings.jitter = std::max(0.0, mSettings.jitter);

  mJitterDistribution = std::uniform_real_distribution<double>(
      -mSettings.jitter, mSettings.jitter);
}

bool GameNet::NetworkTransportSimulationProxy::SendPacket(
    const SocketAddress& dest, std::span<const uint8_t> payload) {
  if (!mEndpoint) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: endpoint is null\n",
                __FUNCTION__);
    return false;
  }

  FlushScheduledOutgoingPackets();

  if (ShouldDropPacket()) {
    return true;  // Packet is intentionally dropped.
  }

  ScheduledOutgoingPacket packet;
  packet.sendTime = ComputeDeliveryTimePoint();
  packet.dest = dest;
  packet.payload.assign(payload.begin(), payload.end());

  mScheduledOutgoingPackets.push_back(std::move(packet));

  return true;
}

bool GameNet::NetworkTransportSimulationProxy::PollPacket(
    NetworkReceivedPacket& recvPacket) {
  if (!mEndpoint) {
    Logger::Log(LOG_SEVERITY_ERROR, "%s error: endpoint is null\n",
                __FUNCTION__);
    return false;
  }

  FlushScheduledOutgoingPackets();
  PumpUnderlyingIncomingPackets();

  if (mScheduledIncomingPackets.empty()) {
    return false;
  }

  const steady_clock::time_point currTime = GetCurrentTimePoint();
  if (mScheduledIncomingPackets.front().deliverTime > currTime) {
    return false;
  }

  recvPacket = std::move(mScheduledIncomingPackets.front().recvPacket);
  mScheduledIncomingPackets.pop_front();
  return true;
}

void GameNet::NetworkTransportSimulationProxy::FlushScheduledOutgoingPackets() {
  const steady_clock::time_point currTime = GetCurrentTimePoint();

  // Sort by send time to ensure deterministic delivery if entries were pushed
  // out-of-order due to jitter.
  std::sort(mScheduledOutgoingPackets.begin(), mScheduledOutgoingPackets.end(),
            [](const ScheduledOutgoingPacket& left,
               const ScheduledOutgoingPacket& right) {
              return left.sendTime < right.sendTime;
            });

  while (!mScheduledOutgoingPackets.empty() &&
         mScheduledOutgoingPackets.front().sendTime <= currTime) {
    ScheduledOutgoingPacket packet =
        std::move(mScheduledOutgoingPackets.front());
    mScheduledOutgoingPackets.pop_front();
    mEndpoint->SendPacket(packet.dest, packet.payload);
  }
}

void GameNet::NetworkTransportSimulationProxy::PumpUnderlyingIncomingPackets() {
  // Pull all currently available packets from the underlying endpoint and
  // schedule them for delivery.
  NetworkReceivedPacket recvPacket;
  while (mEndpoint->PollPacket(recvPacket)) {
    if (ShouldDropPacket()) {
      continue;
    }

    ScheduledIncomingPacket incomingPacket;
    incomingPacket.deliverTime = ComputeDeliveryTimePoint();
    incomingPacket.recvPacket = std::move(recvPacket);

    mScheduledIncomingPackets.push_back(std::move(incomingPacket));
    recvPacket = NetworkReceivedPacket{};
  }

  // Sort by delivery time to allow jitter-driven reordering.
  std::sort(mScheduledIncomingPackets.begin(), mScheduledIncomingPackets.end(),
            [](const ScheduledIncomingPacket& left,
               const ScheduledIncomingPacket& right) {
              return left.deliverTime < right.deliverTime;
            });
}

GameNet::NetworkTransportSimulationProxy::steady_clock::time_point
GameNet::NetworkTransportSimulationProxy::GetCurrentTimePoint() const {
  return steady_clock::now();
}

GameNet::NetworkTransportSimulationProxy::steady_clock::time_point
GameNet::NetworkTransportSimulationProxy::ComputeDeliveryTimePoint() {
  const double jitterOffset = (mSettings.jitter != 0.0)
                                  ? mJitterDistribution(mRandomNumberGenerator)
                                  : 0.0;

  double totalDelay = mSettings.baseLatency + jitterOffset;

  if (totalDelay < 0.0) {
    totalDelay = 0.0;
  }

  const auto delay = std::chrono::duration<double, std::milli>(totalDelay);
  return GetCurrentTimePoint() +
         std::chrono::duration_cast<steady_clock::duration>(delay);
}

bool GameNet::NetworkTransportSimulationProxy::ShouldDropPacket() {
  const double randomProbability =
      mProbabilityDistribution(mRandomNumberGenerator);
  return randomProbability < mSettings.packetLossProbability;
}
