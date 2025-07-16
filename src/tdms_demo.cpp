#include "ChannelObj.h"
#include "GroupObj.h"
#include "PropertiesObj.h"
#include "RootObj.h"
#include "TDMSUtils.h"
#include <iostream>
#include <memory>
#include <string>

using namespace TDMS;

const std::string filename1 = "test/tdms_test.tdms";
BasePropertyObj *test_prop;
BaseChannelObj *test_channel;

int main() {
  std::cout << "Hello World";

  test_prop = new PropertyObj<std::string>("test prop name", "test prop value");

  std::cout << "Creating Objects \n";
  return 0;
}
