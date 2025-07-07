//
// Program: XML_Analyze_File
//
// Description: For a each XML file in a directory parse it, then analyze its
// XML tree and produce an output report of the analysis.
//
// Dependencies: C++20, PLOG, XML_Lib.
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
  PLOG_INFO << "Analyzing " << fileName;
  const xl::XML xml;
  XML_Analyzer xmlAnalyzer;
  xml.parse(xl::FileSource{ fileName });
  xml.traverse(xmlAnalyzer);
  PLOG_INFO << xmlAnalyzer.dump();
  PLOG_INFO << "Finished " << fileName << ".";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  // Initialise logging.
  init(plog::debug, "XML_Analyze_File.log");
  PLOG_INFO << "XML_Analyze_File started ...";
  // Output XML Lib version and data structure metrics
  PLOG_INFO << xl::XML().version();
  PLOG_INFO << XML_Analyzer::dumpNumericSizes();
  PLOG_INFO << XML_Analyzer::dumpNodeSizes();
  // Analyze XML files.
  for (auto &fileName : Utility::createXMLFileList()) {
    try {
      processXMLFile(fileName);
    } catch (std::exception &ex) {
      PLOG_ERROR << "Error: " << ex.what();
    }
  }
  PLOG_INFO << "XML_Analyze_File exited.";
  exit(EXIT_SUCCESS);
}
