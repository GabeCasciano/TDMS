#ifndef CHANNEL_OBJ_H_
#define CHANNEL_OBJ_H_

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "PropertiesObj.h"
#include "TDMSObj.h"

namespace TDMS {

class BaseChannelObj : public TDMSObj {
public:
  BaseChannelObj(std::string path, std::vector<BasePropertyObj> properties)
      : TDMSObj(path, properties) {
    auto pb = TDMSObj::getPathBytes();
    bytes.insert(bytes.end(), pb.begin(), pb.end());

    // convert and insert no properties
    auto propb = TDMSObj::getPropertiesBytes();
    bytes.insert(bytes.end(), propb.begin(), propb.end());
  }

  template<typename T> void addData(std::vector<T> data){
    this->_addData(reinterpret_cast<void *>(data), data.size(), sizeof(T));    
  };

  std::vector<char> getBytes() { return bytes; }

protected:
  virtual std::vector<uint8_t> getRawDataIndex();
  virtual void _addData(void * data, uint32_t count, size_t size);
};

template <typename T>
class ChannelObj : public BaseChannelObj{

  ChannelObj(std::string path, std::vector<BasePropertyObj> properties);

  std::vector<uint8_t> getRawDataIndex() {

    // The overall length of the index
    uint32_t temp = 20; // always unless string
    auto tb = data_to_bytes<uint32_t>(temp);

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
    auto db = data_to_bytes<uint32_t>(temp);
    b.insert(b.end(), db.begin(), db.end());

    // get the array dimension as bytes
    auto adb = data_to_bytes<uint32_t>(ARRAY_DIMENSION);
    b.insert(b.end(), adb.begin(), adb.end());

    // get the number of values and insert
    uint64_t nv = data.size();
    auto nvb = data_to_bytes<uint64_t>(nv);
    b.insert(b.end(), nvb.begin(), nvb.end());

    return b;
  }

  private:
  std::vector<T> data;
};

} // namespace TDMS
#endif
