
// https://www.ni.com/en/support/documentation/supplemental/07/tdms-file-format-internal-structure.html

#include "tdms_writer.h"
#include <cstring>
#include <stdexcept>

#define SAMPLE_RATE 2000
#define WRITE_INT 5
#define AUTO_FLUSH_THRESHOLD SAMPLE_RATE *WRITE_INT

#define kTocMetaData (1L << 1)
#define kTocRawData (1L << 3)
#define kTocNewObjList (1L << 2)

const uint32_t TDMS_TAG = 0x54444D53;
const uint32_t TDMS_VERSION = 4713;

typedef enum {
  tdsTypeVoid,
  tdsTypeI8,
  tdsTypeI16,
  tdsTypeI32,
  tdsTypeI64,
  tdsTypeU8,
  tdsTypeU16,
  tdsTypeU32,
  tdsTypeU64,
  tdsTypeSingleFloat,
  tdsTypeDoubleFloat,
  tdsTypeExtendedFloat,
  tdsTypeSingleFloatWithUnit = 0x19,
  tdsTypeDoubleFloatWithUnit,
  tdsTypeExtendedFloatWithUnit,
  tdsTypeString = 0x20,
  tdsTypeBoolean = 0x21,
  tdsTypeTimeStamp = 0x44,
  tdsTypeFixedPoint = 0x4F,
  tdsTypeComplexSingleFloat = 0x08000c,
  tdsTypeComplexDoubleFloat = 0x10000d,
  tdsTypeDAQmxRawData = 0xFFFFFFFF
} tdsDataType;

TDMSWriter::TDMSWriter(const std::string &filename)
    : filename(filename), metadataWritten(false) {
  file.open(filename, std::ios::binary | std::ios::trunc | std::ios::out);
  if (!file)
    throw std::runtime_error("Unable to open file: " + filename);
}

TDMSWriter::~TDMSWriter() {
  finalize();
  if (file.is_open())
    file.close();
}

void TDMSWriter::addChannel(const std::string &group,
                            const std::string &channel) {
  std::string fullPath = "/" + group + "/" + channel;
  if (channels.count(fullPath))
    throw std::runtime_error("Channel already exists: " + fullPath);
  channels[fullPath] = {};
  channelOrder.push_back(fullPath);
}

void TDMSWriter::addProperty(const std::string &objectPath,
                             const std::string &key, const std::string &value) {
  objectProperties[objectPath][key] = value;
}

void TDMSWriter::writeData(const std::string &group, const std::string &channel,
                           const std::vector<double> &data) {
  std::string fullPath = "/" + group + "/" + channel;
  if (!channels.count(fullPath))
    throw std::runtime_error("Channel does not exist: " + fullPath);
  buffer[fullPath] = data;

  if (buffer[fullPath].size() >= AUTO_FLUSH_THRESHOLD) {
    flush();
  }
}

void TDMSWriter::appendData(const std::string &group,
                            const std::string &channel,
                            const std::vector<double> &data) {
  std::string fullPath = "/" + group + "/" + channel;
  if (!channels.count(fullPath))
    throw std::runtime_error("Channel does not exist: " + fullPath);
  buffer[fullPath].insert(buffer[fullPath].end(), data.begin(), data.end());

  if (buffer[fullPath].size() >= AUTO_FLUSH_THRESHOLD) {
    flush();
  }
}

void TDMSWriter::flush() {
  uint64_t totalRawSize = 0;
  for (const auto &ch : channelOrder)
    totalRawSize += buffer[ch].size() * sizeof(double);

  if (totalRawSize == 0)
    return;

  writeLeadIn(static_cast<uint32_t>(totalRawSize),
              kTocMetaData | kTocRawData | kTocNewObjList);

  writeUInt32(static_cast<uint32_t>(channelOrder.size()));
  for (const auto &path : channelOrder)
    writeMetadata(path);

  for (const auto &path : channelOrder) {
    writeRawData(buffer[path]);
    buffer[path].clear();
  }
}

void TDMSWriter::finalize() { flush(); }

void TDMSWriter::writeLeadIn(uint32_t dataSize, uint32_t tocMask) {
  file.write("TDSm", 4);
  writeUInt32(tocMask);
  writeUInt32(TDMS_VERSION);
  writeUInt64(0);
  writeUInt64(static_cast<uint64_t>(dataSize));
}

void TDMSWriter::writeMetadata(const std::string &path) {
  writeUInt32(static_cast<uint32_t>(path.size()));
  file.write(path.c_str(), path.size());

  writeUInt32(tdsTypeDoubleFloat);
  writeUInt32(1); // raw data present
  writeUInt64(buffer[path].size());

  const auto &props = objectProperties[path];
  writeUInt32(static_cast<uint32_t>(props.size()));

  for (const auto &[key, val] : props) {
    writeUInt32(static_cast<uint32_t>(key.size()));
    file.write(key.c_str(), key.size());
    writeUInt32(tdsTypeString);
    writeUInt32(static_cast<uint32_t>(val.size()));
    file.write(val.c_str(), val.size());
  }
}

void TDMSWriter::writeRawData(const std::vector<double> &data) {
  file.write(reinterpret_cast<const char *>(data.data()),
             data.size() * sizeof(double));
}

void TDMSWriter::writeUInt32(uint32_t val) {
  file.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

void TDMSWriter::writeUInt64(uint64_t val) {
  file.write(reinterpret_cast<const char *>(&val), sizeof(val));
}
