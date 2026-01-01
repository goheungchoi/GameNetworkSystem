#include <bit>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace GameNet {

class OutputMemoryBitStream;
class InputMemoryBitStream;

template <typename T = void>
struct BitStreamWriter {};

template <>
struct BitStreamWriter<void> {};

template <typename T = void>
struct BitStreamReader {};

template <>
struct BitStreamReader<void> {};

namespace detail {

enum class BitStreamType {
  None,
  Writer,
  Reader,
};

template <typename T, typename = void>
struct has_free_writer : std::false_type {};

template <typename T>
struct has_free_writer<
    T, std::void_t<decltype(std::declval<GameNet::BitStreamWriter<T>>()(
           std::declval<OutputMemoryBitStream&>(), std::declval<const T&>()))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_member_write : std::false_type {};

template <typename T>
struct has_member_write<T,
                        std::void_t<decltype(std::declval<T&>().WriteBitStream(
                            std::declval<OutputMemoryBitStream&>()))>>
    : std::true_type {};

template <typename T>
constexpr bool has_free_writer_v = has_free_writer<T>::value;

template <typename T>
constexpr bool has_member_write_v = has_member_write<T>::value;

template <typename T>
constexpr bool has_write_method_any_v =
    has_free_writer_v<T> || has_member_write_v<T>;

template <typename T, typename = void>
struct has_free_reader : std::false_type {};

template <typename T>
struct has_free_reader<
    T, std::void_t<decltype(std::declval<GameNet::BitStreamReader<T>>()(
           std::declval<InputMemoryBitStream&>(), std::declval<T&>()))>>
    : std::true_type {};

template <typename T, typename = void>
struct has_member_read : std::false_type {};

template <typename T>
struct has_member_read<T, std::void_t<decltype(std::declval<T&>().ReadBitStream(
                              std::declval<InputMemoryBitStream&>()))>>
    : std::true_type {};

template <typename T>
constexpr bool has_free_reader_v = has_free_reader<T>::value;

template <typename T>
constexpr bool has_member_read_v = has_member_read<T>::value;

template <typename T>
constexpr bool has_read_method_any_v =
    has_free_reader_v<T> || has_member_read_v<T>;

}  // namespace detail

class OutputMemoryBitStream {
 public:
  OutputMemoryBitStream();
  OutputMemoryBitStream(uint32_t bitCapacity);

  void WriteBits(uint8_t data, uint32_t bitCount);
  void WriteBits(const void* data, uint32_t bitCount);

  const uint8_t* GetBuffer() const { return mBuffer.data(); }
  uint32_t GetBitLength() const { return mBitHead; }
  uint32_t GetByteLength() const { return (mBitHead + 7) >> 3; }

  void WriteBytes(const void* data, uint32_t byteCount) {
    WriteBits(data, byteCount << 3);
  }

  template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T> &&
                                             !(std::is_arithmetic_v<T> ||
                                               std::is_enum_v<T>) &&
                                             !detail::has_write_method_any_v<T>,
                                         bool> = true>
  void Write(const T& data, uint32_t bitCount = sizeof(T) << 3) {
    WriteBits(&data, bitCount);
  }

  template <
      typename T,
      std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, bool> = true>
  void Write(T data, uint32_t bitCount = sizeof(T) << 3) {
    WriteBits(&data, bitCount);
  }

  void Write(bool data) {
    // explicitly convert to 0/1
    uint8_t bit = data ? 1u : 0u;
    WriteBits(bit, 1);
  }

  void Write(float data) {
    uint32_t bits = std::bit_cast<uint32_t>(data);
    Write(bits);
  }

  void Write(double data) {
    uint64_t bits = std::bit_cast<uint64_t>(data);
    Write(bits);
  }

  template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T> &&
                                             !(std::is_arithmetic_v<T> ||
                                               std::is_enum_v<T>) &&
                                             detail::has_write_method_any_v<T>,
                                         bool> = true>
  void Write(const T& data) {
    if constexpr (detail::has_member_write_v<T>) {
      data.WriteBitStream(*this);
    } else {
      BitStreamWriter<T>()(*this, data);
    }
  }

  template <typename T, std::enable_if_t<!std::is_trivially_copyable_v<T> &&
                                             detail::has_write_method_any_v<T>,
                                         bool> = true>
  void Write(const T& data) {
    if constexpr (detail::has_member_write_v<T>) {
      data.WriteBitStream(*this);
    } else {
      BitStreamWriter<T>()(*this, data);
    }
  }

  template <typename T,
            std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>,
                             bool> = true>
  void BitSerialize(T data) {
    Write(data);
  }

  template <typename T,
            std::enable_if_t<!(std::is_arithmetic_v<T> || std::is_enum_v<T>),
                             bool> = true>
  void BitSerialize(const T& data) {
    Write(data);
  }

  template <typename T,
            std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>,
                             bool> = true>
  void BitSerialize(T data, uint32_t bitCount) {
    Write(data, bitCount);
  }

  template <typename T,
            std::enable_if_t<!(std::is_arithmetic_v<T> || std::is_enum_v<T>),
                             bool> = true>
  void BitSerialize(const T& data, uint32_t bitCount) {
    Write(data, bitCount);
  }

 private:
  void ReallocBuffer(uint32_t newBitCapacity);

  uint32_t mBitHead;
  uint32_t mBitCapacity;
  std::vector<uint8_t> mBuffer;
};

class InputMemoryBitStream {
 public:
  InputMemoryBitStream();
  InputMemoryBitStream(uint32_t bitCapacity);
  InputMemoryBitStream(std::vector<uint8_t>&& buffer);

  void ReadBits(uint8_t& outData, uint32_t bitCount);
  void ReadBits(void* outData, uint32_t bitCount);

  uint8_t* GetBuffer() { return mBuffer.data(); }
  const uint8_t* GetBuffer() const { return mBuffer.data(); }
  uint32_t GetBitLength() const { return mBitHead; }
  uint32_t GetByteLength() const { return (mBitHead + 7) >> 3; }
  uint32_t GetBitCapacity() const { return mBitCapacity; }
  uint32_t GetByteCapacity() const { return (mBitCapacity + 7) >> 3; }

  void ReadBytes(void* outData, uint32_t byteCount) {
    ReadBits(outData, byteCount << 3);
  }

  template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T> &&
                                             !std::is_floating_point_v<T> &&
                                             !detail::has_read_method_any_v<T>,
                                         bool> = true>
  void Read(T& outData, uint32_t bitCount = sizeof(T) << 3) {
    ReadBits(&outData, bitCount);
  }

  void Read(bool& outData) {
    // explicitly convert to 0/1
    uint8_t bit;
    ReadBits(bit, 1);
    outData = bit ? true : false;
  }

  void Read(float& outData) {
    uint32_t bits;
    Read(bits);
    outData = std::bit_cast<float>(bits);
  }

  void Read(double& outData) {
    uint64_t bits;
    Read(bits);
    outData = std::bit_cast<double>(bits);
  }

  template <typename T, std::enable_if_t<std::is_trivially_copyable_v<T> &&
                                             !(std::is_arithmetic_v<T> ||
                                               std::is_enum_v<T>) &&
                                             detail::has_read_method_any_v<T>,
                                         bool> = true>
  void Read(T& outDate) {
    if constexpr (detail::has_member_read_v<T>) {
      outDate.ReadBitStream(*this);
    } else {
      BitStreamReader<T>()(*this, outDate);
    }
  }

  template <typename T, std::enable_if_t<!std::is_trivially_copyable_v<T> &&
                                             detail::has_read_method_any_v<T>,
                                         bool> = true>
  void Read(T& outDate) {
    if constexpr (detail::has_member_read_v<T>) {
      outDate.ReadBitStream(*this);
    } else {
      BitStreamReader<T>()(*this, outDate);
    }
  }

  template <typename T>
  void BitSerialize(T& outData) {
    Read(outData);
  }

  template <typename T>
  void BitSerialize(T& outData, uint32_t bitCount) {
    Read(outData, bitCount);
  }

 private:
  std::vector<uint8_t> mBuffer;
  uint32_t mBitHead;
  uint32_t mBitCapacity;
};

}  // namespace GameNet
