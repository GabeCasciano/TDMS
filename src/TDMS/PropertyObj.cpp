#include "PropertiesObj.h"
#include "TDMSUtils.h"
#include <concepts>
#include <cstdint>
#include <stdexcept>

namespace TDMS {

BasePropertyObj::BasePropertyObj(std::string name) : name(name) {
  dType = tdsTypeVoid;
}

template <typename T>
PropertyObj<T>::PropertyObj(std::string name, T value)
    : BasePropertyObj(name), value(value) {

  if constexpr (std::same_as<T, uint8_t>)
    dType = tdsTypeU8;
  else if constexpr (std::same_as<T, int8_t>)
    dType = tdsTypeI8;
  else if constexpr (std::same_as<T, uint16_t>)
    dType = tdsTypeU16;
  else if constexpr (std::same_as<T, int16_t>)
    dType = tdsTypeI16;
  else if constexpr (std::same_as<T, uint32_t>)
    dType = tdsTypeU32;
  else if constexpr (std::same_as<T, int32_t>)
    dType = tdsTypeI32;
  else if constexpr (std::same_as<T, uint64_t>)
    dType = tdsTypeU64;
  else if constexpr (std::same_as<T, int64_t>)
    dType = tdsTypeI64;
  else if constexpr (std::same_as<T, float>)
    dType = tdsTypeSingleFloat;
  else if constexpr (std::same_as<T, double>)
    dType = tdsTypeDoubleFloat;
  else if constexpr (std::same_as<T, std::string>)
    dType = tdsTypeString;

  auto name_str = string_to_bytes(name);
  // insert it
  bytes.insert(bytes.end(), name_str.begin(), name_str.end());

  // preapre the data type uint32_t
  auto dt = std::bit_cast<std::array<char, sizeof(uint32_t)>>(dType);
  // insert it
  bytes.insert(bytes.end(), dt.begin(), dt.end());

  switch (dType) {
  case tdsDataType::tdsTypeBoolean:
  case tdsDataType::tdsTypeI8:
    bytes.push_back(*reinterpret_cast<char *>(value));
    break;
  case tdsDataType::tdsTypeU8:
    bytes.push_back(*reinterpret_cast<char *>(value));
    break;
  case tdsDataType::tdsTypeI16: {
    auto b = data_to_bytes<int16_t>(value);
    bytes.insert(bytes.end(), b.begin(), b.end());
  } break;
  case tdsDataType::tdsTypeU16: {
    auto b = data_to_bytes<uint16_t>(value);
    bytes.insert(bytes.end(), b.begin(), b.end());
  } break;
  case tdsDataType::tdsTypeI32: {
    auto b = data_to_bytes<int32_t>(value);
    bytes.insert(bytes.end(), b.begin(), b.end());
  } break;
  case tdsDataType::tdsTypeU32: {
    auto b = data_to_bytes<uint32_t>(value);
    bytes.insert(bytes.end(), b.begin(), b.end());
  } break;
  case tdsDataType::tdsTypeSingleFloat:
  case tdsDataType::tdsTypeSingleFloatWithUnit: {
    auto b = data_to_bytes<float>(value);
    bytes.insert(bytes.end(), b.begin(), b.end());
  } break;
  case tdsDataType::tdsTypeDoubleFloat:
  case tdsDataType::tdsTypeDoubleFloatWithUnit: {
    auto b = data_to_bytes<double>(value);
    bytes.insert(bytes.end(), b.begin(), b.end());
  } break;
  case tdsDataType::tdsTypeString: {
    auto str = string_to_bytes(value);
    bytes.insert(bytes.end(), str.begin(), str.end());
  } break;
  default:
    throw std::runtime_error(
        "TDMS Writer Error, Unexpected datatype and value");
  }
}

} // namespace TDMS
