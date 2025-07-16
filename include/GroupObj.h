#ifndef GROUP_OBJ_H
#define GROUP_OBJ_H_

#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <vector>

#include "ChannelObj.h"
#include "PropertiesObj.h"
#include "TDMSObj.h"
#include "TDMSUtils.h"

namespace TDMS {

class GroupObj : TDMSObj {
public:
  std::vector<TDMSObj> channels;
  std::vector<BasePropertyObj> properties;

  template <typename T> inline void addProperty(std::string name, T value) {
    properties.push_back(
        new PropertyObj<T>(std::format("{}/{}", path, name), value));
  }
  template <typename T>
  inline void addChannel(std::string name, T type,
                         std::vector<BasePropertyObj> channels) {
    channels.push_back(new ChannelObj<T>(std::format("{}/{}", path, name), ));
  }
};

} // namespace TDMS
#endif
