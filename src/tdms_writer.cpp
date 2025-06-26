
// https://www.ni.com/en/support/documentation/supplemental/07/tdms-file-format-internal-structure.html

#include "tdms_writer.h"
#include <array>
#include <bit>
#include <cstdint>
#include <ios>
#include <stdexcept>
#include <string>
#include <sys/types.h>

std::vector<char> TDMS::string_to_bytes(std::string value) {
  auto len = static_cast<uint32_t>(value.length());
  auto _b = std::bit_cast<std::array<char, sizeof(uint32_t)>>(len);

  std::vector<char> bytes;
  bytes.insert(bytes.end(), _b.begin(), _b.end());
  return bytes;
}

template <typename T> std::array<char, sizeof(T)> TDMS::data_to_bytes(void *v) {
  return std::bit_cast<std::array<char, sizeof(T)>>(*reinterpret_cast<T *>(v));
}

TDMS::PropertyObj(const std::string name, const tdsDataType dType, void *value)
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
// TDMSWriter::TDMSWriter(const std::string filename, const RootObj root_obj) :
// filename(filename){
//   file.open(filename, std::ios::binary | std::ios::trunc | std::ios::out);
//
//   if(!file)
//     throw std::runtime_error("Unable to open file: " + filename);
//
//   writeSegment(root_obj);
// }
//
// TDMSWriter::~TDMSWriter() {
//   finalize();
//   if (file.is_open())
//     file.close();
// }
//
// void TDMSWriter::writeData(){
//
// }
//
// void TDMSWriter::flush() {
//   // uint64_t totalRawSize = 0;
//   // for (const auto &ch : channelOrder)
//   //   totalRawSize += buffer[ch].size() * sizeof(double);
//   //
//   // if (totalRawSize == 0)
//   //   return;
//   //
//   // writeLeadIn(static_cast<uint32_t>(totalRawSize),
//   //             kTocMetaData | kTocRawData | kTocNewObjList);
//   //
//   // writeUInt32(static_cast<uint32_t>(channelOrder.size()));
//   // for (const auto &path : channelOrder)
//   //   writeMetadata(path);
//   //
//   // for (const auto &path : channelOrder) {
//   //   writeRawData(buffer[path]);
//   //   buffer[path].clear();
//   // }
// }
//
// void TDMSWriter::finalize() { flush(); }
//
// template<typename T>
// void  TDMSWriter::pushData(const T &val, std::vector<char> &buffer){
//   auto bytes = std::bit_cast<std::array<char, sizeof(T)>>(val);
//   buffer.insert(buffer.end(), bytes.begin(), bytes.end());
// }
//
//
// void  TDMSWriter::pushString(const std::string &val, std::vector<char>
// &buffer){
//     // Need to ensure that all strings between / and / are enclosed by ' '
//     // I could simply check if '/' exsists and replace / with '/'
//     //  I need to make sure that the first and the last are removed
//     uint32_t len = static_cast<uint32_t>(val.size());
//     auto len_bytes = std::bit_cast<std::array<char, sizeof(len)>>(len);
//
//     buffer.insert(buffer.end(), len_bytes.begin(), len_bytes.end());
//     buffer.insert(buffer.end(), val.begin(), val.end());
// }
//
// void  TDMSWriter::pushLeadIn(uint32_t kToc, uint64_t segment_offset, uint64_t
// raw_data_offset){
//
//   }
//
//
// /*
//  * [0] - Object Path : string
//  * [1] - Raw Data Index : uint32_t
//  *        -> No Raw Data = 0xFFFFFFFF
//  *        -> New Raw Data :
//  *          -> Length : uint32_t
//  *          -> DataType : (uint32_t) tdsDataType
//  *          -> Array Dims : 1
//  *          -> Number of values : uint64_t
//  *          -> Size in bytes : uint64_t (for strings)
//  * [2] - Number of Properties
//  * [3] - Properties:
//  *          -> Name : string
//  *          -> DataType : (uint32_t) tdsDataType
//  *          -> Value :
//  *
//  *
//  */
//
//
// void  TDMSWriter::pushProperty(const PropertyObj &prop){
//   pushString(prop.name, meta_data_buffer);
//   pushData(prop.dType, meta_data_buffer);
//   pushData(prop., meta_data_buffer);
// }
//
// void  TDMSWriter::pushChannelObj(const ChannelObj channel){
//
// }
//
// void  TDMSWriter::pushGroupObj(const GroupObj group){
//
// }
//
// void  TDMSWriter::pushRootObj(const RootObj root){
//
//   meta_obj_count = 0;
//
//   for(auto &obj : root.properties)
//     pushProperty(obj);
//
//   meta_obj_count += root.properties.size();
//
//   for(auto &obj : root.groups){
//     meta_obj_count += obj.properties.size();
//     for(auto &prop : obj.properties)
//       pushProperty(prop);
//   }
// }
//
// void  TDMSWriter::writeSegment(const RootObj data){
//   pushRootObj(data);
//
//   uint32_t kToc = kTocMetaData | kTocNewObjList;
//
//   if(data.groups[0].channels.size() >= 0)
//     kToc |= kTocRawData;
//
//   file.write(reinterpret_cast<char *>(TDMS_TAG), sizeof(uint32_t));
//   file.write(kToc, sizeof(uint32_t));
//   file.write((uint64_t)TDMS_VERSION, sizeof(uint64_t));
//   file.write((uint64_t)data_buffer.size(), sizeof(uint64_t));
//   file.write((uint64_t)meta_data_buffer.size(), sizeof(uint64_t));
//
//
//
//
//   // write leadin
//   // write number of metaobjects aka groups
//   // write each group
//   // write the data
//   // special consideration if there is no data
//
// }
