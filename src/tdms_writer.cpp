
#include "tdms_writer.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <stdint.h>

#define SAMPLE_RATE 2000
#define WRITE_INT 5
#define AUTO_FLUSH_THRESHOLD SAMPLE_RATE * WRITE_INT

const uint32_t TDMS_TAG = 0x54444D53;
const uint32_t TDMS_VERSION = 4713;

TDMSWriter::TDMSWriter(const std::string& filename)
    : filename(filename), metadataWritten(false) {
    file.open(filename, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!file) throw std::runtime_error("Unable to open file: " + filename);
}

TDMSWriter::~TDMSWriter() {
    flush();
    finalize();
    if (file.is_open()) file.close();
}

void TDMSWriter::addChannel(const std::string& group, const std::string& channel) {
    std::string fullPath = "/" + group + "/" + channel;
    if (channels.count(fullPath)) throw std::runtime_error("Channel already exists: " + fullPath);
    channels[fullPath] = {};
    channelOrder.push_back(fullPath);
}

void TDMSWriter::addProperty(const std::string& objectPath, const std::string& key, const std::string& value) {
    objectProperties[objectPath][key] = value;
}

void TDMSWriter::writeData(const std::string& group, const std::string& channel, const std::vector<double>& data) {
    std::string fullPath = "/" + group + "/" + channel;
    if (!channels.count(fullPath)) throw std::runtime_error("Channel does not exist: " + fullPath);
    channels[fullPath] = data;
}

void TDMSWriter::appendData(const std::string& group, const std::string& channel, const std::vector<double>& data) {
    std::string fullPath = "/" + group + "/" + channel;
    if (!channels.count(fullPath)) throw std::runtime_error("Channel does not exist: " + fullPath);
    buffer[fullPath].insert(buffer[fullPath].end(), data.begin(), data.end());

    if (buffer[fullPath].size() >= AUTO_FLUSH_THRESHOLD) {
        flush();
    }
}

void TDMSWriter::flush() {
    uint64_t totalRawSize = 0;
    for (const auto& ch : channelOrder)
        totalRawSize += buffer[ch].size() * sizeof(double);

    if (totalRawSize == 0) return;

    writeLeadIn(static_cast<uint32_t>(totalRawSize));

    if (!metadataWritten) {
        writeUInt32(static_cast<uint32_t>(channelOrder.size()));
        for (const auto& path : channelOrder)
            writeMetadata(path);
        metadataWritten = true;
    } else {
        writeUInt32(0);
    }

    for (const auto& path : channelOrder) {
        writeRawData(buffer[path]);
        buffer[path].clear();
    }
}

void TDMSWriter::finalize() {
    flush();
    for (const auto& [path, data] : channels) {
        writeLeadIn(static_cast<uint32_t>(data.size() * sizeof(double)));
        writeUInt32(1);
        writeMetadata(path);
        writeRawData(data);
    }
}

void TDMSWriter::writeLeadIn(uint32_t dataSize) {
    file.write("TDSm", 4);
    writeUInt32(0x00000001);
    writeUInt32(TDMS_VERSION);
    writeUInt64(0);
    writeUInt64(static_cast<uint64_t>(dataSize));
}

void TDMSWriter::writeMetadata(const std::string& path) {
    writeUInt32(static_cast<uint32_t>(path.size()));
    file.write(path.c_str(), path.size());
    writeUInt32(0x00000021);
    writeUInt32(0x0C);
    writeUInt32(1);
    writeUInt64(0);

    const auto& props = objectProperties[path];
    writeUInt32(static_cast<uint32_t>(props.size()));

    for (const auto& [key, val] : props) {
        writeUInt32(static_cast<uint32_t>(key.size()));
        file.write(key.c_str(), key.size());
        writeUInt32(0x20);
        writeUInt32(static_cast<uint32_t>(val.size()));
        file.write(val.c_str(), val.size());
    }
}

void TDMSWriter::writeRawData(const std::vector<double>& data) {
    for (double val : data)
        file.write(reinterpret_cast<const char*>(&val), sizeof(double));
}

void TDMSWriter::writeUInt32(uint32_t val) {
    file.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

void TDMSWriter::writeUInt64(uint64_t val) {
    file.write(reinterpret_cast<const char*>(&val), sizeof(val));
}
