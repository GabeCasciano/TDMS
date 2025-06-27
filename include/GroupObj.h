#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "ChannelObj.h"
#include "PropertiesObj.h"
#include "TDMSObj.h"
#include "TDMSUtils.h"

namespace TDMS {

class GroupObj : TDMSObj {
public:
  std::vector<std::unique_ptr<TDMSObj>> channels;
};

} // namespace TDMS
