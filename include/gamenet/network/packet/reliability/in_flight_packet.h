#pragma once

#include "reliability_common.h"
#include "transmission_data.h"

namespace GameNet {
class DeliveryNotificationManager;

class InFlightPacket {
 public:
  InFlightPacket(PacketSequenceNumber inSequenceNumber);

  PacketSequenceNumber GetSequenceNumber() const { return mSequenceNumber; }
  float GetTimeDispatched() const { return mTimeDispatched; }

  void SetTransmissionData(int inKey, TransmissionDataPtr inTransmissionData) {
    mTransmissionDataMap[inKey] = inTransmissionData;
  }
  const TransmissionDataPtr GetTransmissionData(int inKey) const {
    auto it = mTransmissionDataMap.find(inKey);
    return (it != mTransmissionDataMap.end()) ? it->second : nullptr;
  }

  void HandleDeliveryFailure(
      DeliveryNotificationManager* inDeliveryNotificationManager) const;
  void HandleDeliverySuccess(
      DeliveryNotificationManager* inDeliveryNotificationManager) const;

 private:
  PacketSequenceNumber mSequenceNumber;
  float mTimeDispatched;

  std::unordered_map<int, TransmissionDataPtr> mTransmissionDataMap;
};
}  // namespace GameNet