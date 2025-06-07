#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

class TDMSWriter {
public:
  TDMSWriter(const std::string &filename);
  ~TDMSWriter();

  void addChannel(const std::string &group, const std::string &channel);
  void addProperty(const std::string &objectPath, const std::string &key,
                   const std::string &value);
  void writeData(const std::string &group, const std::string &channel,
                 const std::vector<double> &data);
  void appendData(const std::string &group, const std::string &channel,
                  const std::vector<double> &data);
  void flush();
  void finalize();

private:
  void writeLeadIn(uint32_t dataSize, uint32_t tocMask);
  void writeMetadata(const std::string &path);
  void writeRawData(const std::vector<double> &data);
  void writeUInt32(uint32_t val);
  void writeUInt64(uint64_t val);

  std::ofstream file;
  std::string filename;
  bool metadataWritten;

  std::unordered_map<std::string, std::vector<double>> channels;
  std::unordered_map<std::string, std::vector<double>> buffer;
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
      objectProperties;
  std::vector<std::string> channelOrder;
};
