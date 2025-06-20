#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

#define kTocMetaData (1L << 1)     // Segment contains meta data
#define kTocRawData (1L << 3)      // Segment contains raw data
#define kTocDAQmxRawData (1L << 7) // Segment contains DAQmx raw data
#define kTocInterleavedData                                                    \
  (1L << 5) // Raw data in the segment is interleaved (if flag is not set, data
            // is contiguous)
#define kTocBigEndian                                                          \
  (1L << 6) // All numeric values in the segment, including the lead in, raw
            // data, and meta data, are big-endian formatted (if flag is not
            // set, data is little-endian). ToC is not affected by endianess; it
            // is always little-endian.
#define kTocNewObjList (1L << 2)

namespace TDMS {

const uint32_t TDMS_TAG = 0x54444D53;
const uint32_t TDMS_VERSION = 4713;

const uint32_t NO_DATA = 0xFFFFFFFF;
const uint32_t DUPLICATE_DATA = 0x00000000;
const uint32_t ARRAY_DIMENSION = 1;

typedef enum tdsDataType {
  tdsTypeVoid = 0x00,
  tdsTypeI8 = 0x01,            // int8_t
  tdsTypeI16 = 0x02,           // int16_t
  tdsTypeI32 = 0x03,           // int32_t
  tdsTypeI64 = 0x04,           // int64_t
  tdsTypeU8 = 0x05,            // uint8_t
  tdsTypeU16 = 0x06,           // uint16_t
  tdsTypeU32 = 0x07,           // uint32_t
  tdsTypeU64 = 0x08,           // uint64_t
  tdsTypeSingleFloat = 0x09,   // float
  tdsTypeDoubleFloat = 0x0A,   // double
  tdsTypeExtendedFloat = 0x0B, // long double (not always supported)
  tdsTypeSingleFloatWithUnit = 0x19,
  tdsTypeDoubleFloatWithUnit = 0x1A,
  tdsTypeString = 0x20,  // Length-prefixed UTF-8 string
  tdsTypeBoolean = 0x21, // uint8_t 0=false, 1=true
  tdsTypeTimeStamp =
      0x44, // Two int64_t values (seconds and fractional seconds)
  tdsTypeFixedPoint = 0x4F,
  tdsTypeComplexSingleFloat = 0x08000c,
  tdsTypeComplexDoubleFloat = 0x10000d,
  tdsTypeDAQmxRawData = 0xFFFFFFFF
} tdsDataType;

std::vector<uint8_t> string_to_bytes(std::string value) {
  auto len = static_cast<uint32_t>(value.length());
  auto _b = std::bit_cast<std::array<uint8_t, sizeof(uint32_t)>>(len);

  std::vector<uint8_t> bytes;
  bytes.insert(bytes.end(), _b.begin(), _b.end());
  return bytes;
}

template <typename T> std::array<uint8_t, sizeof(T)> data_to_bytes(void *v) {
  return std::bit_cast<std::array<uint8_t, sizeof(T)>>(
      *reinterpret_cast<T *>(v));
}

void push_vec(std::vector<uint8_t> &dest, std::vector<uint8_t> &src) {
  dest.insert(dest.end(), src.begin(), src.end());
}

template <std::size_t N>
void push_vec(std::vector<uint8_t> &dest, const std::array<uint8_t, N> &src) {
  dest.insert(dest.end(), src.begin(), src.end());
}

class PropertyObj {
public:
  std::string name;
  tdsDataType dType;
  void *value;
  std::vector<uint8_t> bytes;

  PropertyObj(const std::string name, const tdsDataType dType, void *value)
      : name(name), dType(dType), value(value) {

    // using string to bytes helper convert name to correct format
    auto name_str = string_to_bytes(name);
    push_vec(bytes, name_str);

    // preapre the data type uint32_t
    auto dt = data_to_bytes<uint32_t>((void *)&dType);
    push_vec(bytes, dt);

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
      push_vec(bytes, b);
      break;
    }
    case tdsDataType::tdsTypeU16: {
      auto b = data_to_bytes<uint16_t>(value);
      push_vec(bytes, b);
      break;
    }
    case tdsDataType::tdsTypeI32: {
      auto b = data_to_bytes<int32_t>(value);
      push_vec(bytes, b);
      break;
    }
    case tdsDataType::tdsTypeU32: {
      auto b = data_to_bytes<uint32_t>(value);
      push_vec(bytes, b);
      break;
    }
    case tdsDataType::tdsTypeSingleFloat:
    case tdsDataType::tdsTypeSingleFloatWithUnit: {
      auto b = data_to_bytes<float>(value);
      push_vec(bytes, b);
      break;
    }
    case tdsDataType::tdsTypeDoubleFloat:
    case tdsDataType::tdsTypeDoubleFloatWithUnit: {
      auto b = data_to_bytes<double>(value);
      push_vec(bytes, b);
      break;
    }
    default:
      throw std::runtime_error(
          "TDMS Writer Error, Unexpected datatype and value");
    }
  }

  PropertyObj(const std::string name, const std::string value) : name(name) {
    // using string to bytes helper convert name to correct format
    auto name_str = string_to_bytes(name);
    push_vec(bytes, name_str);

    // preapre the data type uint32_t
    auto dt =
        std::bit_cast<std::array<uint8_t, sizeof(uint32_t)>>(tdsTypeString);
    push_vec(bytes, dt);

    auto value_str = string_to_bytes(value);
    push_vec(bytes, value_str);
  }

  std::vector<uint8_t> getBytes() { return bytes; };
};

template <typename T> class RawDataObj {
public:
  std::vector<T> raw_data;
};

class TDMSObj {
public:
  std::string path;
  std::vector<PropertyObj> properties;
  std::vector<uint8_t> bytes;

  /* The binary layour for a TDMS object is as follows
   *
   * Object Path : string
   * Raw Data Index :
   *    Length : uint32_t
   *    Data Type : uint32_t
   *    Array Dim : uint32_t always value 1
   *    Numbre of Values : uint64_t
   *    Size in Bytes : uint64_t
   * Number of Properties : uint32_t
   * Properties
   *    Name : string
   *    Data Type : uint32_t
   *    Value : dType
   */
  TDMSObj(std::string path, std::vector<PropertyObj> properties)
      : path(path), properties(properties) {

    // To-do: need to make sure to convert /group/... to /'group'/...

    // Convert and insert the path name
    auto pb = getPathBytes();
    push_vec(bytes, pb);

    // Convert and insert the no data designator
    auto db = data_to_bytes<uint32_t>((void *)&NO_DATA);
    push_vec(bytes, db);

    // Convert and instert the number of properties
    auto propb = getPropertiesBytes();
    push_vec(bytes, propb);
  }

  std::vector<uint8_t> getPathBytes() {
    uint32_t plen = path.length();
    auto plb = data_to_bytes<uint32_t>((void *)&plen);
    auto pb = string_to_bytes(path);

    std::vector<uint8_t> b;
    b.insert(b.end(), plb.begin(), plb.end());
    b.insert(b.end(), pb.begin(), pb.end());
    return b;
  };
  std::vector<uint8_t> getPropertiesBytes() {
    uint32_t pCount = properties.size();
    auto pcb = data_to_bytes<uint32_t>((void *)&pCount);
    std::vector<uint8_t> b;
    b.insert(b.end(), pcb.begin(), pcb.end());

    for (PropertyObj &prop : properties) {
      auto prb = prop.getBytes();
      b.insert(b.end(), prb.begin(), prb.end());
    }
    return b;
  };
  std::vector<uint8_t> getBytes() { return bytes; }
};

template <typename T> class ChannelObj : TDMSObj {
public:
  std::vector<T> data;

  ChannelObj(std::string path, std::vector<PropertyObj> properties)
      : TDMSObj(path, properties) {}
  ChannelObj(std::string path, std::vector<T> data,
             std::vector<PropertyObj> properties)
      : TDMSObj(path, properties), data(data) {
    // convert and insert the path
    auto pb = TDMSObj::getPathBytes();
    push_vec(bytes, pb);

    // convert and insert the raw data index
    auto db = getRawDataIndex();
    push_vec(bytes, db);

    // convert and insert no properties
    auto propb = TDMSObj::getPropertiesBytes();
    push_vec(bytes, propb);
  }

  std::vector<uint8_t> getRawDataIndex() {

    // The overall length of the index
    uint32_t temp = 20; // always unless string
    auto tb = data_to_bytes<uint32_t>((void *)&temp);

    std::vector<uint8_t> b;
    push_vec(b, tb);

    bool is_string = false;

    // determine what data type
    if constexpr (std::is_same_v<T, uint8_t>)
      temp = static_cast<uint32_t>(tdsTypeU8);
    else if constexpr (std::is_same_v<T, int8_t>)
      temp = static_cast<uint32_t>(tdsTypeI8);
    else if constexpr (std::is_same_v<T, uint16_t>)
      temp = static_cast<uint32_t>(tdsTypeU16);
    else if constexpr (std::is_same_v<T, int16_t>)
      temp = static_cast<uint32_t>(tdsTypeI16);
    else if constexpr (std::is_same_v<T, uint32_t>)
      temp = static_cast<uint32_t>(tdsTypeU32);
    else if constexpr (std::is_same_v<T, int32_t>)
      temp = static_cast<uint32_t>(tdsTypeI32);
    else if constexpr (std::is_same_v<T, float>)
      temp = static_cast<uint32_t>(tdsTypeSingleFloat);
    else if constexpr (std::is_same_v<T, double>)
      temp = static_cast<uint32_t>(tdsTypeDoubleFloat);
    else if constexpr (std::is_same_v<T, std::string>) {
      temp = static_cast<uint32_t>(tdsTypeString);
      is_string = true; // need to do something with this
    }

    // write the data type
    auto db = data_to_bytes<uint32_t>((void *)&temp);
    push_vec(b, db);

    // get the array dimension as bytes
    auto adb = data_to_bytes<uint32_t>((void *)&ARRAY_DIMENSION);
    push_vec(b, adb);

    // get the number of values and insert
    uint64_t nv = data.size();
    auto nvb = data_to_bytes<uint64_t>((void *)nv);
    push_vec(b, nvb);

    return b;
  }

  std::vector<uint8_t> getBytes() { return bytes; }
};

template <typename T> class GroupObj : TDMSObj {
public:
  std::vector<ChannelObj<T>> channels;

  GroupObj(std::string path, std::vector<PropertyObj> properties,
           std::vector<ChannelObj<T>> channels)
      : TDMSObj(path, properties), channels(channels) {
    auto pb = getPathBytes();
    push_vec(bytes, pb);
  }
};

class FileObj {
public:
  std::vector<PropertyObj> properties;
  std::vector<TDMSObj> groups;
};

class TDMSWriter {
public:
  TDMSWriter(const std::string filename, const FileObj root_obj);
  ~TDMSWriter();

  void writeSegment(const FileObj data);

  void flush();
  void finalize();

private:
  void writeData();

  void pushString(const std::string &val, std::vector<uint8_t> &buffer);
  void pushData(const auto &val, std::vector<uint8_t> &buffer);

  void pushProperty(const PropertyObj prop);
  void pushFileObj(const FileObj root);
  void pushGroupObj(const TDMSObj group);
  void pushChannelObj(const TDMSObj channel);

  std::vector<char> meta_data_buffer, data_buffer;
  uint32_t meta_obj_count = 0;

  std::ofstream file;
  std::string filename;
};

} // namespace TDMS
