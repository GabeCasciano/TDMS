#include "tdms_writer.h"
#include <iostream>

const std::string filename1 = "test/tdms_test.tdms";
const std::string filename2 = "test/tdms_test2.tdms";
const int dummy_sample = 100;

TDMSWriter *test_writer;
std::vector<double> dummy_data;

int main() {
  std::cout << "Hello World";

  std::cout << "Creating new TDMS File, name:" << filename1 << " \n";
  test_writer = new TDMSWriter(filename1);

  std::cout << "Adding TestGroup and TestChannel \n";
  test_writer->addChannel("TestGroup", "TestChannel");

  std::cout << "Adding TestProperty: TestPropertyValue to TestGroup \n";
  test_writer->addProperty("/TestGroup", "TestProperty", "TestPropertyValue");

  test_writer->finalize();

  std::cout << "Generating dummy_data \n";
  for (int i = 0; i < dummy_sample; i++)
    dummy_data.push_back(i);

  std::cout << "Adding dummy_data to TestGroup, TestChannel \n";
  test_writer->writeData("TestGroup", "TestChannel", dummy_data);

  std::cout << "Closing file, it should flush and make the file \n";
  delete test_writer;

  return 0;
}
