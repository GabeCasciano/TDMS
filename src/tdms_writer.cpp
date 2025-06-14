
// https://www.ni.com/en/support/documentation/supplemental/07/tdms-file-format-internal-structure.html

#include "tdms_writer.h"
#include <cstdint>
#include <ios>
#include <stdexcept>
#include <string>
#include <sys/types.h>

TDMSWriter::TDMSWriter(const std::string filename, const RootObj root_obj) : filename(filename){
  file.open(filename, std::ios::binary | std::ios::trunc | std::ios::out);

  if(!file)
    throw std::runtime_error("Unable to open file: " + filename);
  
  writeSegment(root_obj);
}

TDMSWriter::~TDMSWriter() {
  finalize();
  if (file.is_open())
    file.close();
}

void TDMSWriter::writeData(){

}

void TDMSWriter::flush() {
  // uint64_t totalRawSize = 0;
  // for (const auto &ch : channelOrder)
  //   totalRawSize += buffer[ch].size() * sizeof(double);
  //
  // if (totalRawSize == 0)
  //   return;
  //
  // writeLeadIn(static_cast<uint32_t>(totalRawSize),
  //             kTocMetaData | kTocRawData | kTocNewObjList);
  //
  // writeUInt32(static_cast<uint32_t>(channelOrder.size()));
  // for (const auto &path : channelOrder)
  //   writeMetadata(path);
  //
  // for (const auto &path : channelOrder) {
  //   writeRawData(buffer[path]);
  //   buffer[path].clear();
  // }
}

void TDMSWriter::finalize() { flush(); }

void  TDMSWriter::pushData(const auto val, std::vector<char> *buffer){
  buffer->push_back(static_cast<char>(val, sizeof(val)));
}

void  TDMSWriter::pushString(const std::string val, std::vector<char> *buffer){
  
  uint32_t len = static_cast<uint32_t>(val.size());

  buffer->insert(buffer->end(),
              reinterpret_cast<const char*>(&len),
              reinterpret_cast<const char*>(&len) + sizeof(len));

  buffer->insert(buffer->end(), val.begin(), val.end());
}

void  TDMSWriter::pushLeadIn(uint32_t kToc){
  }


/*
 * [0] - Object Path : string 
 * [1] - Raw Data Index : uint32_t
 *        -> No Raw Data = 0xFFFFFFFF
 *        -> New Raw Data :
 *          -> Length : uint32_t 
 *          -> DataType : (uint32_t) tdsDataType
 *          -> Array Dims : 1 
 *          -> Number of values : uint64_t
 *          -> Size in bytes : uint64_t (for strings)
 * [2] - Number of Properties
 * [3] - Properties:
 *          -> Name : string 
 *          -> DataType : (uint32_t) tdsDataType
 *          -> Value : 
 *
 *
 */


void  TDMSWriter::pushProperty(const PropertyObj prop){
  pushString(prop.name, &meta_data_buffer);
  pushData((uint32_t *) 0xFFFFFFFF, &meta_data_buffer);
  pushData((uint32_t *) 1, &meta_data_buffer);
}

void  TDMSWriter::pushChannelObj(const ChannelObj channel){

}

void  TDMSWriter::pushGroupObj(const GroupObj group){
  
}

void  TDMSWriter::pushRootObj(const RootObj root){

  meta_obj_count = 0;

  for(auto &obj : root.properties)
    pushProperty(obj);
  
  meta_obj_count += root.properties.size();

  for(auto &obj : root.groups){
    meta_obj_count += obj.properties.size();
    for(auto &prop : obj.properties)
      pushProperty(prop);
  }
}

void  TDMSWriter::writeSegment(const RootObj data){
  pushRootObj(data);
  
  uint32_t kToc = kTocMetaData | kTocNewObjList;

  if(data.groups[0].channels.size() >= 0)
    kToc |= kTocRawData;

  file.write(reinterpret_cast<char *>(TDMS_TAG), sizeof(uint32_t));
  file.write(kToc, sizeof(uint32_t));
  file.write((uint64_t)TDMS_VERSION, sizeof(uint64_t));
  file.write((uint64_t)data_buffer.size(), sizeof(uint64_t));
  file.write((uint64_t)meta_data_buffer.size(), sizeof(uint64_t));


   

  // write leadin 
  // write number of metaobjects aka groups
  // write each group
  // write the data
  // special consideration if there is no data
  
}
