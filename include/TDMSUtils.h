#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace TDMS {

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

std::vector<uint8_t> string_to_byte(std::string value) {
  std::vector<uint8_t> bytes;

  return bytes;
};

template <typename T> std::vector<uint8_t> data_to_bytes(T data) {
  std::array<uint8_t, sizeof(T)> bytes;

  return bytes;
};

} // namespace TDMS
