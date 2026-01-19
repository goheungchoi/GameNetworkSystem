#pragma once

#include <chrono>
#include <deque>
#include <memory>
#include <random>
#include <span>

#include "endpoint/network_endpoint_interface.h"

namespace GameNet {

struct NetworkTransportSimulationSettings {
  double baseLatency = 0.0; // in milliseconds
  double jitter = 0.0;  // in milliseconds
  double packetLossProbability = 0.0;  // in the range [0.0, 1.0].
};

class NetworkTransportSimulationProxy final : public INetworkTransportEndpoint {
  public:
  explicit NetworkTransportSimulationProxy(
    std::unique_ptr<INetworkTransportEndpoint> endpoint,
    NetworkTransportSimulationSettings settings
  );

  NetworkTransportSimulationProxy(const NetworkTransportSimulationProxy&) = delete;
  NetworkTransportSimulationProxy& operator=(const NetworkTransportSimulationProxy&) = delete;

  bool SendPacket(const SocketAddress& dest, std::span<const uint8_t> payload) override;

  bool PollPacket(NetworkReceivedPacket& recvPacket) override;

  SocketAddress GetLocalSocketAddress() const override {
    return mEndpoint->GetLocalSocketAddress();
  }

  private:
  using steady_clock = std::chrono::steady_clock;

  struct ScheduledOutgoingPacket {
    steady_clock::time_point sendTime;
    SocketAddress dest;
    std::vector<uint8_t> payload;
  };

  struct ScheduledIncomingPacket {
    steady_clock::time_point deliverTime;
    NetworkReceivedPacket recvPacket;
  };

  void FlushScheduledOutgoingPackets();
  void PumpUnderlyingIncomingPackets();
  
  steady_clock::time_point GetCurrentTimePoint() const;
  steady_clock::time_point ComputeDeliveryTimePoint();

  bool ShouldDropPacket();

  std::unique_ptr<INetworkTransportEndpoint> mEndpoint;
  NetworkTransportSimulationSettings mSettings;

  std::deque<ScheduledOutgoingPacket> mScheduledOutgoingPackets;
  std::deque<ScheduledIncomingPacket> mScheduledIncomingPackets;

  std::mt19937 mRandomNumberGenerator;
  std::uniform_real_distribution<double> mProbabilityDistribution;
  std::uniform_real_distribution<double> mJitterDistribution;
};

}