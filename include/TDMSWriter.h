#pragma once

#include <cstdint>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

#include "TDMSUtils.h"

namespace TDMS {

class TDMSWriter {
public:
  TDMSWriter(const std::string filename, const FileObj root_obj);
  ~TDMSWriter();

  void writeDataOnlySegment(const std::vector<std::vector<float>> data);
  void writeSegment(const FileObj data);

  void flush();
  void finalize();

private:
  void writeData();

  void pushString(const std::string &val, std::vector<char> &buffer);
  void pushData(const auto &val, std::vector<char> &buffer);

  void pushProperty(const PropertyObj prop);
  void pushFileObj(const FileObj root);
  void pushGroupObj(const GroupObj group);
  void pushChannelObj(const ChannelObj channel);

  std::vector<char> meta_data_buffer, data_buffer;
  uint32_t meta_obj_count = 0;

  std::ofstream file;
  std::string filename;
};

} // namespace TDMS
