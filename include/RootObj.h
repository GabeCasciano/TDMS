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

class RootObj : TDMSObj{
public:
  std::vector<GroupObj> groups;

  RootObj(GroupObj groups, BasePropertyObj properties);
};

} // namespace TDMS
