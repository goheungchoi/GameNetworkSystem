#pragma once

#include "reliability_common.h"

namespace GameNet {

class DeliveryNotificationManager;

class TransmissionData {
 public:
  virtual void HandleDeliveryFailure(
      DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
  virtual void HandleDeliverySuccess(
      DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
};
using TransmissionDataPtr = std::shared_ptr<TransmissionData>;

}  // namespace GameNet
