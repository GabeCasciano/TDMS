#include "GroupObj.h"
#include "PropertiesObj.h"
#include "RootObj.h"
#include "TDMSUtils.h"
#include <iostream>
#include <memory>

using namespace TDMS;

const std::string filename1 = "test/tdms_test.tdms";

class TestSensor{
  std::vector<std::unique_ptr<BasePropertyObj>> properties;

  TestSensor(std::string desc, std::string name){
    properties.push_back(std::make_unique<PropertyObj<std::string>>("Description", desc));

  }
}


int main() {
  std::cout << "Hello World";

  std::cout << "Creating Objects \n";
  return 0;
}
