#include "reliability/ack_range.h"

bool GameNet::AckRange::MaybePushBack(PacketSequenceNumber sequenceNumber) {
  // Exceeded the max value.
  if (mCount >= std::numeric_limits<uint16_t>::max()) {
    return false;
  }

  if (sequenceNumber == mStart + mCount) {
    ++mCount;
    return true;
  } else {
    return false;
  }
}

void GameNet::AckRange::WriteBitStream(
    OutputMemoryBitStream& outputStream) const {
  outputStream.Write(mStart);
  bool hasCount = mCount > 1;
  outputStream.Write(hasCount);
  if (hasCount) {
    outputStream.Write(mCount - 1);
  }
}

void GameNet::AckRange::ReadBitStream(InputMemoryBitStream& inputStream) {
  inputStream.Read(mStart);
  bool hasCount{false};
  inputStream.Read(hasCount);
  if (hasCount) {
    inputStream.Read(mCount);
  } else {
    // default fallback.
    mCount = 1;
  }
}
