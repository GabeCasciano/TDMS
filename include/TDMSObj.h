#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "PropertiesObj.h"
#include "TDMSUtils.h"

namespace TDMS {
class TDMSObj {
public:
  std::string path;
  std::vector<std::unique_ptr<BasePropertyObj>> properties;
  std::vector<uint8_t> bytes;

  /* The binary layour for a TDMS object is as follows
   *
   * Object Path : string
   * Raw Data Index :
   *    Length : uint32_t
   *    Data Type : uint32_t
   *    Array Dim : uint32_t always value 1
   *    Numbre of Values : uint64_t
   *    Size in Bytes : uint64_t
   * Number of Properties : uint32_t
   * Properties
   *    Name : string
   *    Data Type : uint32_t
   *    Value : dType
   */
  TDMSObj(std::string path, std::vector<std::unique_ptr<BasePropertyObj>> properties)
      : path(path), properties(std::move(properties)) {

    // To-do: need to make sure to convert /group/... to /'group'/...

    // Convert and insert the path name
    auto pb = getPathBytes();
    bytes.insert(bytes.end(), pb.begin(), pb.end());

    // Convert and insert the no data designator
    auto db = data_to_bytes<uint32_t>(NO_DATA);
    bytes.insert(bytes.end(), db.begin(), db.end());

    // Convert and instert the number of properties
    auto propb = getPropertiesBytes();
    bytes.insert(bytes.end(), propb.begin(), propb.end());
  }

  std::vector<uint8_t> getPathBytes() {
    uint32_t plen = path.length();
    auto plb = data_to_bytes<uint32_t>(plen);
    auto pb = string_to_bytes(path);

    std::vector<uint8_t> b;
    b.insert(b.end(), plb.begin(), plb.end());
    b.insert(b.end(), pb.begin(), pb.end());
    return b;
  };
  std::vector<uint8_t> getPropertiesBytes() {
    uint32_t pCount = properties.size();
    auto pcb = data_to_bytes<uint32_t>(pCount);
    std::vector<uint8_t> b;
    b.insert(b.end(), pcb.begin(), pcb.end());

    for(int i = 0; i < properties.size(); i++){
      auto prb = properties[i]->getBytes();
      b.insert(b.end(), prb.begin(), prb.end());
    }
    return b;
  };
  std::vector<uint8_t> getBytes() { return bytes; }
};
} // namespace TDMS
