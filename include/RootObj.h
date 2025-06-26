#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ChannelObj.h"
#include "GroupObj.h"
#include "PropertiesObj.h"
#include "TDMSObj.h"
#include "TDMSUtils.h"

namespace TDMS {

class FileObj {
public:
  std::vector<PropertyObj> properties;
  std::vector<GroupObj> groups;
};

} // namespace TDMS
