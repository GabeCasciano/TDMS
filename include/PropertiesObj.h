#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "TDMSUtils.h"

namespace TDMS {

class PropertyObj {
public:
  std::string name;
  tdsDataType dType;
  void *value;
  std::vector<char> bytes;

  PropertyObj(const std::string name, const tdsDataType dType, void *value);
  std::vector<char> getBytes() { return bytes; };
};

} // namespace TDMS
