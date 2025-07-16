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
  std::vector<BasePropertyObj> properties;

  RootObj(std::string name);

  inline void addGroupObj(GroupObj group){}
  inline void addGroupObj(std::string name, std::vector<BasePropertyObj> properties){}
  inline void addGroupObj(std::string name, std::vector<BasePropertyObj> properties, std::vector<BaseChannelObj> channels){}

};

} // namespace TDMS
