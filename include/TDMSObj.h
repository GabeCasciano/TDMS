#ifndef TDMSOBJ_H_
#define TDMSOBJ_H_

#include <algorithm>
#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include "PropertiesObj.h"
#include "TDMSUtils.h"

namespace TDMS {
class TDMSObj {
public:
  std::string path;
  std::vector<BasePropertyObj> properties;
  std::vector<char> bytes;

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
  TDMSObj(std::string path, std::vector<BasePropertyObj> properties)
      : path(path) {

    // Make a copy of the properties that are passed.
    for (auto &p : properties)
      this->properties.push_back(p);

    // To-do: need to make sure to convert /group/... to /'group'/...
    updateBytes();
  }
  TDMSObj() = default;
  ~TDMSObj() {}

  inline void updateBytes() {
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

  template <typename T> inline void addProperty(std::string name, T value) {
    properties.push_back(
        new PropertyObj<T>(std::format("{}/{}", path, name), value));
    updateBytes();
  }

  inline void addProperty(BasePropertyObj property){
    properties.push_back(property);
    updateBytes();
  }

  inline void addProperties(std::vector<BasePropertyObj> properties){
    for(auto &p: properties)
      properties.push_back(p);
    updateBytes();
  }

  inline std::string getPath() { return path; }
  inline std::vector<BasePropertyObj> getProperties() { return properties; }
  inline std::vector<char> getBytes() { return bytes; }

  inline std::vector<char> getPathBytes() {
    uint32_t plen = path.length();
    auto plb = data_to_bytes<uint32_t>(plen);
    auto pb = string_to_bytes(path);

    std::vector<char> b;
    b.insert(b.end(), plb.begin(), plb.end());
    b.insert(b.end(), pb.begin(), pb.end());
    return b;
  };

  inline std::vector<char> getPropertiesBytes() {
    uint32_t pCount = properties.size();
    auto pcb = data_to_bytes<uint32_t>(pCount);
    std::vector<char> b;
    b.insert(b.end(), pcb.begin(), pcb.end());

    for (int i = 0; i < properties.size(); i++) {
      auto prb = properties[i].getBytes();
      b.insert(b.end(), prb.begin(), prb.end());
    }
    return b;
  };
};
}// namespace TDMS
#endif
