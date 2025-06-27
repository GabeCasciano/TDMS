#include "TDMSUtils.h"

namespace TDMS{

template <typename T> std::array<uint8_t, sizeof(T)> data_to_bytes(T value) {
  return std::bit_cast<std::array<uint8_t, sizeof(T)>>(reinterpret_cast<T>(value));
}

std::vector<uint8_t> string_to_bytes(std::string value) {
  uint32_t len = static_cast<uint32_t>(value.length());
  auto _b = std::bit_cast<std::array<uint8_t, sizeof(uint32_t)>>(len);

  std::vector<uint8_t> bytes;

  bytes.insert(bytes.end(), _b.begin(), _b.end());
  bytes.insert(bytes.end(), value.begin(), value.end());
  return bytes;
}


}
