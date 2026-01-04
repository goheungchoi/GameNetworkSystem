#pragma once

#include <cinttypes>
#include <cstring>

namespace GameNet {

class CircularBuffer {
  const bool kGrowable;
  const size_t kBlockSize;

  struct buf_pointer {
    size_t n, m;

    bool operator==(const buf_pointer& other);
    bool operator!=(const buf_pointer& other);
    bool operator<(const buf_pointer& other);
  };

  size_t _totalSize;
  size_t _totalCapacity;
  buf_pointer _hd, _tl;

  size_t _tableCapacity;
  uint8_t** _table;

 public:
  CircularBuffer(size_t blockSize = 2048, bool growable = true);
  ~CircularBuffer();

  CircularBuffer(const CircularBuffer& other);
  CircularBuffer& operator=(const CircularBuffer& other);

  CircularBuffer(CircularBuffer&& other) noexcept;
  CircularBuffer& operator=(CircularBuffer&& other) noexcept;

  bool Write(const void* p, size_t size);

  bool Read(void* p, size_t size);

  bool Peek(void* p, size_t size) const;

  template <typename T>
  bool Write(const T* p) {
    return Write(p, sizeof(T));
  }

  template <typename T>
  bool Read(T* p) {
    return Read(p, sizeof(T));
  }

  template <typename T>
  bool Peek(T* p) {
    return Peek(p, sizeof(T));
  }

  bool Empty() const;

  size_t Size() const;
  size_t Capacity() const;

 private:
  void InitTable();

  void CleanupTable();

  void Copy(const GameNet::CircularBuffer& other);

  void IncreaseTable();

  void DecreaseTable();

  buf_pointer AdvanceBufferPointer(buf_pointer pointer, size_t size) const;
};

}  // namespace nx