#include "tdms_writer.h"
#include <iostream>

const std::string filename1 = "test/tdms_test.tdms";
const std::string filename2 = "test/tdms_test2.tdms";
const int dummy_sample = 5;

TDMSWriter *test_writer;

PropertyObj testPropObj{
    .name = "TestProp", .dType = tdsTypeString, .value = "TestPropValue"};

RootObj testRootObj;
GroupObj testGroupObj{.name = "TestGroup"};
ChannelObj testChannelObj{.name = "TestChannel"};

int main() {
  std::cout << "Hello World";

  std::cout << "Creating Objects \n";
  testGroupObj.channels.push_back(testChannelObj);
  testGroupObj.properties.push_back(testPropObj);

  testRootObj.properties.push_back(testPropObj);
  testRootObj.groups.push_back(testGroupObj);

  std::cout << "Creating new TDMS File, name:" << filename1 << " \n";
  test_writer = new TDMSWriter(filename1, testRootObj);

  return 0;
}
