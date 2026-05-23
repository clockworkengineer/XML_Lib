//
// Program: XML_Analyze_File
//
// Description: For a each XML file in a directory parse it, then analyze its
// XML tree and produce an output report of the analysis.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"
#include "XML_Analyzer.hpp"

namespace xl = XML_Lib;

/// <summary>
/// Parse XML file and analyze its XML tree.
/// </summary>
/// <param name="fileName">XML file name</param>
void processXMLFile(const std::string &fileName)
{
  std::cout << "Analyzing " << fileName << std::endl;
  const xl::XML xml;
  XML_Analyzer xmlAnalyzer;
  xml.parse(xl::FileSource{ fileName });
  xml.traverse(xmlAnalyzer);
  std::cout << xmlAnalyzer.dump() << std::endl;
  std::cout << "Finished " << fileName << "." << std::endl;
}

/// <summary>
/// Program entry point.
/// </summary>
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  // Initialise logging.
  std::cout << "XML_Analyze_File started ..." << std::endl;
  // Output XML Lib version and data structure metrics
  std::cout << xl::XML().version() << std::endl;
  std::cout << XML_Analyzer::dumpNumericSizes() << std::endl;
  std::cout << XML_Analyzer::dumpNodeSizes() << std::endl;
  // Analyze XML files.
  for (auto &fileName : Utility::createXMLFileList()) {
    try {
      processXMLFile(fileName);
    } catch (std::exception &ex) {
      std::cerr << "Error: " << ex.what() << std::endl;
    }
  }
  std::cout << "XML_Analyze_File exited." << std::endl;
  exit(EXIT_SUCCESS);
}