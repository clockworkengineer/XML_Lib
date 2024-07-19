
//
// Program: XML_Fibonacci
//
// Description: On each activation add the next entry in the fibonacci
// sequence to the array stored in fibonacci.xml; if the file does
// not exist then create the initial sequence of [0,1].
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"
#include "XML.hpp"
#include "XML_Core.hpp"

namespace xl = XML_Lib;

/// <summary>
/// Return Fibonacci xml file name.
/// </summary>
/// <returns>XML settings file name.</returns>
std::string xmlFibonacciFile() { return (std::filesystem::current_path() / "files" / "fibonacci.xml").string(); }
/// <summary>
/// Read in current fibonacci sequence from XML file, calculate the
/// next in sequence and write back to XML file.
/// </summary>
void nextFibonacci()
{
  const xl::XML xml;
  if (!std::filesystem::exists(xmlFibonacciFile())) {
    // If XML file does not exist create initial sequence
    xml.parse(xl::BufferSource{ R"(<root><row>1</row><row>1</row><row>2</row></root>)" });
  } else {
    // Parse in current sequence
    xml.parse(xl::FileSource{ xmlFibonacciFile() });
    // Get index of last element
    const auto last = xml.root().getChildren().size() - 1;
    // Get last two in sequence
    const auto first = std::stol(xl::XRef<xl::Element>(xml.root()[last - 1]).getContents());
    const auto second = std::stol(xl::XRef<xl::Element>(xml.root()[last]).getContents());
    // Create new element for next in sequence
    auto  xNode = xl::XNode::make<xl::Element>("row");
    xNode.addChild(xl::XNode::make<xl::Content>(std::to_string(first+second)));
    xml.root().addChild(xNode);
  }
  // Write updated sequence back to file
  xml.stringify(xl::FileDestination{ xmlFibonacciFile() });
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  try {
    // Initialise logging.
    init(plog::debug, "XML_Fibonacci.log");
    PLOG_INFO << "XML_Fibonacci started ...";
    // Log version
    PLOG_INFO << xl::XML().version();
    // Update current sequence
    nextFibonacci();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "XML_Fibonacci exited.";
  exit(EXIT_SUCCESS);
}