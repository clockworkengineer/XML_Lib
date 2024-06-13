//
// Program: XML_Create_At_Runtime
//
// Description: Create XML programmatically at runtime using a combination
// of the XML class, indexing/key operators plus the use of initializer lists.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"
#include "XML.hpp"
#include "XML_Core.hpp"

namespace xl = XML_Lib;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  try {
    // Initialise logging.
    plog::init(plog::debug, "XML_Create_At_Runtime.log");
    PLOG_INFO << "XML_Create_At_Runtime started ...";
    // Log version
    PLOG_INFO << xl::XML().version();
    // create an empty structure (null)
    xl::XML xml { "<root attrib1='value1' attrib2='value2' > contents </root>"};
    xl::BufferDestination destination;
    xml.stringify(destination);
    PLOG_INFO << "attrib1 = " <<xl::XRef<xl::XElement>(xml.root())["attrib1"].getUnparsed();
    PLOG_INFO << "attrib2 = " << xl::XRef<xl::XElement>(xml.root())["attrib2"].getUnparsed();
    PLOG_INFO << destination.toString();
    xml = "<root attrib1='value1' attrib2='value2' attrib3='value3' attrib4='value4'> contents </root>";
    destination.clear();
    xml.stringify(destination);
    PLOG_INFO << destination.toString();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "XML_Create_At_Runtime exited.";
  exit(EXIT_SUCCESS);
}
