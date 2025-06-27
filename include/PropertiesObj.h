#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "TDMSUtils.h"

namespace TDMS {

class BasePropertyObj { 
public:
  std::string name;
  tdsDataType dType;
  std::vector<uint8_t> bytes;

  BasePropertyObj(std::string name);
  ~BasePropertyObj();
  virtual std::vector<uint8_t> getBytes() { return bytes; };
};

template<typename T> class PropertyObj : BasePropertyObj{
public:
  T value;

  PropertyObj(std::string name, T value);
  std::vector<uint8_t> getBytes(){ return bytes;}

};

} // namespace TDMS
