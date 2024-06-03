
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

namespace xml = XML_Lib;

/// <summary>
/// Return Fibonaci xml file name.
/// </summary>
/// <returns>XML settings file name.</returns>
std::string xmlFibonacciFile() { return (std::filesystem::current_path() / "files" / "fibonacci.xml").string(); }
/// <summary>
/// Read in current fibonacci sequence from XML file, calculate the
//  next in sequence and write back to XML file.
/// </summary>
void nextFibonacci()
{
  xml::XML xml;
  if (!std::filesystem::exists(xmlFibonacciFile())) {
    // If XML file does not exist create intial sequence
    xml.parse(xml::BufferSource{ "[0, 1]" });
  } else {
    // Parse in current sequence
    xml.parse(xml::FileSource{ xmlFibonacciFile() });
    // Get index of last element
    // auto last = xml::XRef<xml::Array>(xml.root()).size() - 1;
    // // Next is sum of last two entries
    // auto next = xml::XRef<xml::Number>(xml[last]).value<int>();
    // next += xml::XRef<xml::Number>(xml[last - 1]).value<int>();
    // // Expand array by one and add next in sequence
    // xml[last + 1] = next;
  }
  // Write updated sequence back to file
  xml.stringify(xml::FileDestination{ xmlFibonacciFile() });
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  try {
    // Initialise logging.
    plog::init(plog::debug, "XML_Fibonacci.log");
    PLOG_INFO << "XML_Fibonacci started ...";
    // Log version
    PLOG_INFO << xml::XML().version();
    // Update current sequence
    nextFibonacci();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "XML_Fibonacci exited.";
  exit(EXIT_SUCCESS);
}