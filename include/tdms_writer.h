
#pragma once
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <stdint.h>

class TDMSWriter {
public:
    TDMSWriter(const std::string& filename);
    ~TDMSWriter();

    void addChannel(const std::string& group, const std::string& channel);
    void addProperty(const std::string& objectPath, const std::string& key, const std::string& value);

    void writeData(const std::string& group, const std::string& channel, const std::vector<double>& data);
    void appendData(const std::string& group, const std::string& channel, const std::vector<double>& data);

    void flush();
    void finalize();

private:
    std::string filename;
    std::ofstream file;

    bool metadataWritten;
    std::unordered_map<std::string, std::vector<double>> channels;
    std::unordered_map<std::string, std::vector<double>> buffer;
    std::vector<std::string> channelOrder;

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> objectProperties;

    void writeLeadIn(uint32_t dataSize);
    void writeMetadata(const std::string& path);
    void writeRawData(const std::vector<double>& data);

    void writeUInt32(uint32_t val);
    void writeUInt64(uint64_t val);
};
