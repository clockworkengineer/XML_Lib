//
// Program: XML_Parse_File
//
// Description: For each XML file in a directory parse it, stringify
// it back into text form and then parse the buffer created; timing each
// step in turn for each file.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"
#include "XML.hpp"
#include "XML_Core.hpp"

namespace xl = XML_Lib;
namespace chrono = std::chrono;

static constexpr size_t kMaxFileLengthToDisplay = 16 * 1024;

/// <summary>
/// Parse XML file, stringify and parse XML from buffer whilst timing each
/// step.
/// </summary>
/// <param name="fileName">XML file name</param>
void processXMLFile(const std::string &fileName)
{
  auto elapsedTime = [](const auto &start, const auto &stop) {
    return chrono::duration_cast<chrono::microseconds>(stop - start).count();
  };
  PLOG_INFO << "Processing " << fileName;
  const xl::XML xml;
  xl::BufferDestination xmlDestination;
  // Parse from file
  auto start = chrono::high_resolution_clock::now();
  xml.parse(xl::FileSource{ fileName });
  auto stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to parse from file.";
  // Stringify to file
  start = chrono::high_resolution_clock::now();
  xml.stringify(xl::FileDestination{ fileName + ".new" });
  stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to stringify to file.";
  // Stringify to buffer
  start = chrono::high_resolution_clock::now();
  xml.stringify(xmlDestination);
  stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to stringify to buffer.";
  // Parse from buffer
  start = chrono::high_resolution_clock::now();
  xml.parse(xl::BufferSource{ xmlDestination.toString() });
  stop = chrono::high_resolution_clock::now();
  PLOG_INFO << elapsedTime(start, stop) << " microseconds to parse from buffer.";
  // Display contents
  if (xmlDestination.toString().size() < kMaxFileLengthToDisplay) { PLOG_INFO << "[" << xmlDestination.toString() << "]"; }
  PLOG_INFO << "--------------------FILE PROCESSED OK--------------------";
  PLOG_INFO << "Finished " << fileName << ".";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  // Initialise logging.
  init(plog::debug, "XML_Parse_Files.log");
  PLOG_INFO << "XML_Parse_Files started ...";
  PLOG_INFO << xl::XML().version();
  // For each xml parse it, stringify it and display unless its to large.
  for (auto &fileName : Utility::createXMLFileList()) {
    try {
      processXMLFile(fileName);
    } catch (std::exception &ex) {
      PLOG_ERROR << "Error: " << ex.what();
    }
  }
  exit(EXIT_SUCCESS);
}