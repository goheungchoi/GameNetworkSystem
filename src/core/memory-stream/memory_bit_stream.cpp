#include "memory-stream/memory_bit_stream.h"

#include <cassert>  // TODO: Replace it with custom assert.

GameNet::OutputMemoryBitStream::OutputMemoryBitStream()
    : OutputMemoryBitStream(1200 << 3) {}

GameNet::OutputMemoryBitStream::OutputMemoryBitStream(uint32_t bitCapacity)
    : mBitHead(0),
      mBitCapacity(bitCapacity),
      mBuffer((mBitCapacity + 7) >> 3, 0u) {}

void GameNet::OutputMemoryBitStream::WriteBits(uint8_t data,
                                               uint32_t bitCount) {
  assert(1 <= bitCount && bitCount <= 8);

  const uint32_t nextBitHead = mBitHead + bitCount;
  if (nextBitHead > mBitCapacity) {
    ReallocBuffer(std::max(mBitCapacity << 1, nextBitHead));
  }

  const uint32_t currByteOffset = mBitHead >> 3;
  const uint32_t currBitOffset = mBitHead & 0x7;

  // Trim input data to exactly bitCount bits.
  data &= static_cast<uint8_t>((1u << bitCount) - 1u);

  const uint32_t bitsFreeThisByte = 8u - currBitOffset;
  const uint32_t bitsThisByte =
      (bitCount < bitsFreeThisByte) ? bitCount : bitsFreeThisByte;

  // Mask for the range we overwrite in the current byte.
  const uint8_t maskThisByte =
      static_cast<uint8_t>(((1u << bitsThisByte) - 1u) << currBitOffset);

  mBuffer[currByteOffset] =
      (mBuffer[currByteOffset] & ~maskThisByte) |
      static_cast<uint8_t>((data << currBitOffset) & maskThisByte);

  // Write the remaining bits into byteOffset + 1.
  if (bitCount > bitsFreeThisByte) {
    const uint32_t remaining = bitCount - bitsFreeThisByte;
    const uint8_t maskNext = static_cast<uint8_t>((1u << remaining) - 1u);

    mBuffer[currByteOffset + 1] =
        (mBuffer[currByteOffset + 1] & ~maskNext) |
        static_cast<uint8_t>((data >> bitsFreeThisByte) & maskNext);
  }

  // Advance bit head.
  mBitHead = nextBitHead;
}

void GameNet::OutputMemoryBitStream::WriteBits(const void* data,
                                               uint32_t bitCount) {
  const uint8_t* srcBytes = static_cast<const uint8_t*>(data);

  // Write all the bytes.
  while (8 < bitCount) {
    WriteBits(*srcBytes, 8);
    ++srcBytes;
    bitCount -= 8;
  }

  // Write anything left.
  if (bitCount) {
    WriteBits(*srcBytes, bitCount);
  }
}

void GameNet::OutputMemoryBitStream::ReallocBuffer(uint32_t newBitCapacity) {
  std::vector<uint8_t> newBuffer((newBitCapacity + 7) >> 3, 0u);
  std::copy(mBuffer.begin(), mBuffer.end(), newBuffer.begin());
  std::swap(mBuffer, newBuffer);
  mBitCapacity = newBitCapacity;
}

GameNet::InputMemoryBitStream::InputMemoryBitStream()
    : InputMemoryBitStream(1200 << 3) {}

GameNet::InputMemoryBitStream::InputMemoryBitStream(uint32_t bitCapacity)
    : mBitHead(0),
      mBitCapacity(bitCapacity),
      mBuffer(((mBitCapacity + 7) >> 3), 0u) {}

GameNet::InputMemoryBitStream::InputMemoryBitStream(
    std::vector<uint8_t>&& buffer)
    : mBitHead(0),
      mBitCapacity(static_cast<uint32_t>(buffer.size()) << 3),
      mBuffer(std::move(buffer)) {}

void GameNet::InputMemoryBitStream::ReadBits(uint8_t& outData,
                                             uint32_t bitCount) {
  assert(1 <= bitCount && bitCount <= 8);

  const uint32_t nextBitHead = mBitHead + bitCount;

  // Avoid buffer overrun if packet is malformed.
  if (nextBitHead > mBitCapacity) {
    outData = 0;
    mBitHead = mBitCapacity;  // clamp
    return;
  }

  const uint32_t currByteOffset = mBitHead >> 3;
  const uint32_t currBitOffset = mBitHead & 0x7;

  // Pull bits from the current byte, aligned down to bit 0.
  uint32_t value =
      static_cast<uint32_t>(mBuffer[currByteOffset]) >> currBitOffset;

  const uint32_t bitsFreeThisByte = 8u - currBitOffset;

  // If the read straddles the next byte, pull remaining bits from next byte.
  if (bitCount > bitsFreeThisByte) {
    // We want the next byte bits to become the higher bits of 'value'.
    value |= static_cast<uint32_t>(mBuffer[currByteOffset + 1])
             << bitsFreeThisByte;
  }

  // Mask to keep only bitCount bits.
  const uint32_t mask =
      (bitCount == 32) ? 0xFFFFFFFFu : ((1u << bitCount) - 1u);
  outData = static_cast<uint8_t>(value & mask);

  mBitHead = nextBitHead;
}

void GameNet::InputMemoryBitStream::ReadBits(void* outData, uint32_t bitCount) {
  uint8_t* dstBytes = static_cast<uint8_t*>(outData);

  // Read whole bytes.
  while (bitCount > 8) {
    ReadBits(*dstBytes, 8);
    ++dstBytes;
    bitCount -= 8;
  }

  // Read leftover bits.
  if (bitCount) {
    ReadBits(*dstBytes, bitCount);
  }
}