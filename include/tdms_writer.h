#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#define kTocMetaData         (1L<<1)	// Segment contains meta data
#define kTocRawData          (1L<<3)	// Segment contains raw data
#define kTocDAQmxRawData     (1L<<7)	// Segment contains DAQmx raw data
#define kTocInterleavedData  (1L<<5)	// Raw data in the segment is interleaved (if flag is not set, data is contiguous)
#define kTocBigEndian        (1L<<6)	// All numeric values in the segment, including the lead in, raw data, and meta data, are big-endian formatted (if flag is not set, data is little-endian). ToC is not affected by endianess; it is always little-endian.
#define kTocNewObjList       (1L<<2)

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

typedef struct{
  std::string name;
  tdsDataType dType = tdsTypeString;
  std::string value;
}PropertyObj;

typedef struct{
  std::string name;
  std::vector<double> data;
}ChannelObj;

typedef struct{
  std::string name;
  std::vector<ChannelObj> channels;
  std::vector<PropertyObj> properties;
}GroupObj;

typedef struct{
  std::vector<PropertyObj> properties;
  std::vector<GroupObj> groups;
}RootObj;   

class TDMSWriter {
public:
  TDMSWriter(const std::string filename, const RootObj root_obj);
  ~TDMSWriter();
  
  void writeSegment(const RootObj data);

  void flush();
  void finalize();

private:

  void writeData();
  
  void pushString(const std::string val, std::vector<char> *buffer);
  void pushData(const auto val, std::vector<char> *buffer);

  void pushProperty(const PropertyObj prop);
  void pushRootObj(const RootObj root);
  void pushGroupObj(const GroupObj group);
  void pushChannelObj(const ChannelObj channel);

  std::vector<char> meta_data_buffer, data_buffer;
  uint32_t meta_obj_count = 0;    

  std::ofstream file;
  std::string filename;
  
};
