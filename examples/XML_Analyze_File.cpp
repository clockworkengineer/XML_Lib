//
// Program: XML_Analyze_File
//
// Description: For a each XML file in a directory parse it, then analyze its
// XML tree and produce a output report of the analysis.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"
#include "XML_Analyzer.hpp"

namespace js = XML_Lib;

/// <summary>
/// Parse XML file and analyze its XML tree.
/// </summary>
/// <param name="fileName">XML file name</param>
void processXMLFile(const std::string &fileName)
{
  PLOG_INFO << "Analyzing " << fileName;
  const js::XML json;
  XML_Analyzer jsonAnalyzer;
  json.parse(js::FileSource{ fileName });
  json.traverse(jsonAnalyzer);
  PLOG_INFO << jsonAnalyzer.dump();
  PLOG_INFO << "Finished " << fileName << ".";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  // Initialise logging.
  plog::init(plog::debug, "XML_Analyze_File.log");
  PLOG_INFO << "XML_Analyze_File started ...";
  // Output XML Lib version and data structure metrics
  PLOG_INFO << js::XML().version();
  PLOG_INFO << XML_Analyzer::dumpNumericSizes();
  PLOG_INFO << XML_Analyzer::dumpXNodeSizes();
  // Analyze XML files.
  for (auto &fileName : Utility::createXMLFileList()) {
    try {
      processXMLFile(fileName);
    } catch (std::exception &ex) {
      PLOG_INFO << ex.what() << "\n";
    }
  }
  PLOG_INFO << "XML_Analyze_File exited.";
  exit(EXIT_SUCCESS);
}
