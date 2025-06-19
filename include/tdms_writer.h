#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <type_traits>
#include <unordered_map>
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

std::vector<char> string_to_bytes(const std::string &str) {
  auto len = static_cast<uint32_t>(str.length());
  auto _b = std::bit_cast<std::array<char, sizeof(uint32_t)>>(len);

  std::vector<char> bytes;
  bytes.insert(bytes.end(), _b.begin(), _b.end());
  return bytes;
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
      auto v = *reinterpret_cast<int16_t *>(value);
      auto b = std::bit_cast<std::array<char, sizeof(int16_t)>>(v);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeU16: {
      auto v = *reinterpret_cast<uint16_t *>(value);
      auto b = std::bit_cast<std::array<char, sizeof(uint16_t)>>(v);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeI32: {
      auto v = *reinterpret_cast<int32_t *>(value);
      auto b = std::bit_cast<std::array<char, sizeof(int32_t)>>(v);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeU32: {
      auto v = *reinterpret_cast<uint32_t *>(value);
      auto b = std::bit_cast<std::array<char, sizeof(uint32_t)>>(v);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeSingleFloat:
    case tdsDataType::tdsTypeSingleFloatWithUnit: {
      auto v = *reinterpret_cast<float *>(value);
      auto b = std::bit_cast<std::array<char, sizeof(float)>>(v);
      bytes.insert(bytes.end(), b.begin(), b.end());
      break;
    }
    case tdsDataType::tdsTypeDoubleFloat:
    case tdsDataType::tdsTypeDoubleFloatWithUnit: {
      auto v = *reinterpret_cast<double *>(value);
      auto b = std::bit_cast<std::array<char, sizeof(double)>>(v);
      bytes.insert(bytes.end(), b.begin(), b.end());
    }
    case tdsDataType::tdsTypeString: {
      auto str = *reinterpret_cast<std::string *>(value);
      auto str_bytes = string_to_bytes(str);
      bytes.insert(bytes.end(), str_bytes.begin(), str_bytes.end());
    }
    default:
      break;
    }
  }

  std::vector<char> getBytes() { return bytes; };
};

class ChannelObj {
public:
  std::string name;
  std::vector<PropertyObj> properties;
  std::vector<double> raw_data;
};

class GroupObj {
public:
  std::string name;
  std::vector<ChannelObj> channels;
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
