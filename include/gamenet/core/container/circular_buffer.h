#pragma once

#include <cinttypes>
#include <cstring>

namespace ctnr {

class circular_buffer {
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
  circular_buffer(size_t blockSize = 2048, bool growable = true);
  ~circular_buffer();

  circular_buffer(const circular_buffer& other);
  circular_buffer& operator=(const circular_buffer& other);

  circular_buffer(circular_buffer&& other) noexcept;
  circular_buffer& operator=(circular_buffer&& other) noexcept;

  bool write(const void* p, size_t size);

  bool read(void* p, size_t size);

  bool peek(void* p, size_t size) const;

  template <typename T>
  bool write(const T* p) {
    return write(p, sizeof(T));
  }

  template <typename T>
  bool read(T* p) {
    return read(p, sizeof(T));
  }

  template <typename T>
  bool peek(T* p) {
    return peek(p, sizeof(T));
  }

  bool empty() const;

  size_t size() const;
  size_t capacity() const;

 private:
  void init_table();

  void cleanup_table();

  void copy(const ctnr::circular_buffer& other);

  void increase_table();

  void decrease_table();

  buf_pointer advance_buf_pointer(buf_pointer pointer, size_t size) const;
};

}  // namespace nx