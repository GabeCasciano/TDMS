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

class FileObj : TDMSObj{
public:
  std::vector<GroupObj> groups;

  FileObj(GroupObj groups, BasePropertyObj properties);
};

} // namespace TDMS
