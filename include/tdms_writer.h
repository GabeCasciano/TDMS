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

const uint32_t TDMS_TAG = 0x54444D53;
const uint32_t TDMS_VERSION = 4713;

const uint32_t NO_DATA = 0xFFFFFFFF;
const uint32_t DUPLICATE_DATA = 0x00000000;
const uint32_t ARRAY_DIMENSION = 1;

typedef enum {
  tdsTypeVoid,
  tdsTypeI8,
  tdsTypeI16,
  tdsTypeI32,
  tdsTypeI64,
  tdsTypeU8,
  tdsTypeU16,
  tdsTypeU32,
  tdsTypeU64,
  tdsTypeSingleFloat,
  tdsTypeDoubleFloat,
  tdsTypeExtendedFloat,
  tdsTypeSingleFloatWithUnit = 0x19,
  tdsTypeDoubleFloatWithUnit,
  tdsTypeExtendedFloatWithUnit,
  tdsTypeString = 0x20,
  tdsTypeBoolean = 0x21,
  tdsTypeTimeStamp = 0x44,
  tdsTypeFixedPoint = 0x4F,
  tdsTypeComplexSingleFloat = 0x08000c,
  tdsTypeComplexDoubleFloat = 0x10000d,
  tdsTypeDAQmxRawData = 0xFFFFFFFF
} tdsDataType;

std::vector<char> string_to_bytes(std::string value) {
  auto len = static_cast<uint32_t>(value.length());
  auto _b = std::bit_cast<std::array<char, sizeof(uint32_t)>>(len);

  std::vector<char> bytes;
  bytes.insert(bytes.end(), _b.begin(), _b.end());
  return bytes;
}

std::vector<char> string_to_bytes(void *value) {
  auto str = *reinterpret_cast<std::string *>(value);
  return string_to_bytes(str);
}

template <typename T> std::array<char, sizeof(T)> data_to_bytes(void *v) {
  return std::bit_cast<std::array<char, sizeof(T)>>(*reinterpret_cast<T *>(v));
}

class PropertyObj {
public:
  std::string name;
  tdsDataType dType;
  void *value;
  std::vector<char> bytes;

  PropertyObj(const std::string name, const tdsDataType dType, void *value)
      : name(name), dType(dType), value(value) {

    // using string to bytes helper convert name to correct format
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
      break;
    }
    case tdsDataType::tdsTypeU16: {
      auto b = data_to_bytes<uint16_t>(value);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeI32: {
      auto b = data_to_bytes<int32_t>(value);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeU32: {
      auto b = data_to_bytes<uint32_t>(value);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeSingleFloat:
    case tdsDataType::tdsTypeSingleFloatWithUnit: {
      auto b = data_to_bytes<float>(value);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeDoubleFloat:
    case tdsDataType::tdsTypeDoubleFloatWithUnit: {
      auto b = data_to_bytes<double>(value);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeString: {
      auto str = string_to_bytes(value);
      bytes.insert(bytes.end(), str.begin(), str.end());
      break;
    }
    default:
      throw std::runtime_error(
          "TDMS Writer Error, Unexpected datatype and value");
    }
  }

  std::vector<char> getBytes() { return bytes; };
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
    bytes.insert(bytes.end(), pb.begin(), pb.end());

    // Convert and insert the no data designator
    auto db = data_to_bytes<uint32_t>((void *)&NO_DATA);
    bytes.insert(bytes.end(), db.begin(), db.end());

    // Convert and instert the number of properties
    auto propb = getPropertiesBytes();
    bytes.insert(bytes.end(), propb.begin(), propb.end());
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
  std::string path;
  std::vector<PropertyObj> properties;
  std::vector<T> data;

  ChannelObj(std::string path, std::vector<PropertyObj> properties)
      : path(path), properties(properties) {
    TDMSObj(path, properties);
  }
  ChannelObj(std::string path, std::vector<T> data) : path(path), data(data) {
    // convert and insert the path
    auto pb = TDMSObj::getPathBytes();
    bytes.insert(bytes.end(), pb.begin(), pb.end());

    // convert and insert the raw data index
    auto db = getRawDataIndex();
    bytes.insert(bytes.end(), db.begin(), db.end());

    // convert and insert no properties
    auto propb = TDMSObj::getPropertiesBytes();
    bytes.insert(bytes.end(), propb.begin(), propb.end());
  }
  ChannelObj(std::string path, std::vector<T> data,
             std::vector<PropertyObj> properties)
      : path(path), data(data), properties(properties) {
    // convert and insert the path
    auto pb = TDMSObj::getPathBytes();
    bytes.insert(bytes.end(), pb.begin(), pb.end());

    // convert and insert the raw data index
    auto db = getRawDataIndex();
    bytes.insert(bytes.end(), db.begin(), db.end());

    // convert and insert no properties
    auto propb = TDMSObj::getPropertiesBytes();
    bytes.insert(bytes.end(), propb.begin(), propb.end());
  }

  std::vector<uint8_t> getRawDataIndex() {

    // The overall length of the index
    uint32_t temp = 20; // always unless string
    auto tb = data_to_bytes<uint32_t>((void *)&temp);

    std::vector<uint8_t> b;
    b.insert(b.end(), tb.begin(), tb.end());

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
    b.insert(b.end(), db.begin(), db.end());

    // get the array dimension as bytes
    auto adb = data_to_bytes<uint32_t>((void *)&ARRAY_DIMENSION);
    b.insert(b.end(), adb.begin(), adb.end());

    // get the number of values and insert
    uint64_t nv = data.size();
    auto nvb = data_to_bytes<uint64_t>((void *)nv);
    b.insert(b.end(), nvb.begin(), nvb.end());

    return b;
  }

  std::vector<uint8_t> getBytes() { return bytes; }
};

template <typename T> class GroupObj {
public:
  std::string path;
  std::vector<ChannelObj<T>> channels;
  std::vector<PropertyObj> properties;
};

class FileObj {
public:
  std::vector<PropertyObj> properties;
  std::vector<GroupObj> groups;
};

class TDMSWriter {
public:
  TDMSWriter(const std::string filename, const FileObj root_obj);
  ~TDMSWriter();

  void writeDataOnlySegment(const std::vector<std::vector<float>> data);
  void writeSegment(const FileObj data);

  void flush();
  void finalize();

private:
  void writeData();

  void pushString(const std::string &val, std::vector<char> &buffer);
  void pushData(const auto &val, std::vector<char> &buffer);

  void pushProperty(const PropertyObj prop);
  void pushFileObj(const FileObj root);
  void pushGroupObj(const GroupObj group);
  void pushChannelObj(const ChannelObj channel);

  std::vector<char> meta_data_buffer, data_buffer;
  uint32_t meta_obj_count = 0;

  std::ofstream file;
  std::string filename;
};
