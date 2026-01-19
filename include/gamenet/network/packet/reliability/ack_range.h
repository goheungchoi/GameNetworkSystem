#pragma once

#include "reliability_common.h"

namespace GameNet {

class AckRange {
 public:
  AckRange() = default;

  AckRange(PacketSequenceNumber startNumber) : mStart(startNumber), mCount(1) {}

  // if this is the next in sequence, just extend the range
  bool MaybePushBack(PacketSequenceNumber sequenceNumber);

  PacketSequenceNumber GetStart() const { return mStart; }
  uint8_t GetCount() const { return mCount; }

  void WriteBitStream(OutputMemoryBitStream& outputStream) const;
  void ReadBitStream(InputMemoryBitStream& inputStream);

 private:
  PacketSequenceNumber mStart{0};
  uint16_t mCount{0};
};

}  // namespace GameNet
