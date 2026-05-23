//
// Program: XML_Parse_File
//
// Description: For each XML file in a directory parse it, stringify
// it back into text form and then parse the buffer created; timing each
// step in turn for each file.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;
namespace chrono = std::chrono;

static constexpr size_t kMaxFileLengthToDisplay = 16 * 1024;

/// @brief
/// Parse XML file, stringify and parse XML from buffer whilst timing each
/// step.

/// @param fileName XML file name
void processXMLFile(const std::string &fileName)
{
  auto elapsedTime = [](const auto &start, const auto &stop) {
    return chrono::duration_cast<chrono::microseconds>(stop - start).count();
  };
  std::cout << "Processing " << fileName << std::endl;
  const xl::XML xml;
  xl::BufferDestination xmlDestination;
  // Parse from file
  auto start = chrono::high_resolution_clock::now();
  xml.parse(xl::FileSource{ fileName });
  auto stop = chrono::high_resolution_clock::now();
  std::cout << elapsedTime(start, stop) << " microseconds to parse from file." << std::endl;
  // Stringify to file
  start = chrono::high_resolution_clock::now();
  xml.stringify(xl::FileDestination{ fileName + ".new" });
  stop = chrono::high_resolution_clock::now();
  std::cout << elapsedTime(start, stop) << " microseconds to stringify to file." << std::endl;
  // Stringify to buffer
  start = chrono::high_resolution_clock::now();
  xml.stringify(xmlDestination);
  stop = chrono::high_resolution_clock::now();
  std::cout << elapsedTime(start, stop) << " microseconds to stringify to buffer." << std::endl;
  // Parse from buffer
  start = chrono::high_resolution_clock::now();
  xml.parse(xl::BufferSource{ xmlDestination.toString() });
  stop = chrono::high_resolution_clock::now();
  std::cout << elapsedTime(start, stop) << " microseconds to parse from buffer." << std::endl;
  // Display contents
  if (xmlDestination.toString().size() < kMaxFileLengthToDisplay) {
    std::cout << "[" << xmlDestination.toString() << "]" << std::endl;
  }
  std::cout << "--------------------FILE PROCESSED OK--------------------" << std::endl;
  std::cout << "Finished " << fileName << "." << std::endl;
}

/// @brief
/// Program entry point.

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  // Initialise logging.
  std::cout << "XML_Parse_Files started ..." << std::endl;
  std::cout << xl::XML().version() << std::endl;
  // For each xml parse it, stringify it and display unless its to large.
  for (auto &fileName : Utility::createXMLFileList()) {
    try {
      processXMLFile(fileName);
    } catch (std::exception &ex) {
      std::cerr << "Error: " << ex.what() << std::endl;
    }
  }
  exit(EXIT_SUCCESS);
}