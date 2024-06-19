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
    // Create XML using constructor
    xl::XML xml { "<root attrib1='value1' attrib2='value2' > contents </root>"};
    xl::BufferDestination destination;
    xml.stringify(destination);
    PLOG_INFO << "attrib1 = " <<xl::XRef<xl::XElement>(xml.root())["attrib1"].getUnparsed();
    PLOG_INFO << "attrib2 = " << xl::XRef<xl::XElement>(xml.root())["attrib2"].getUnparsed();
    PLOG_INFO << destination.toString();
    // Create XML using assignment
    xml = "<root attrib1='value1' attrib2='value2' attrib3='value3' attrib4='value4'> contents </root>";
    destination.clear();
    xml.stringify(destination);
    PLOG_INFO << destination.toString();
    // Add attributes to base XML
    xml = "<root>test content</root>" ;
    XRef<xl::XElement>(xml.root()).addAttribute("attribute1", xl::XMLValue("value1", "value1"));
    destination.clear();
    xml.stringify(destination);
    PLOG_INFO << destination.toString();
    XRef<xl::XElement>(xml.root()).addAttribute("attribute2", xl::XMLValue("value2", "value2"));
    XRef<xl::XElement>(xml.root()).addAttribute("attribute3", xl::XMLValue("value3", "value3"));
    destination.clear();
    xml.stringify(destination);
    PLOG_INFO << destination.toString();
    // Add namespaces to base XML
    xml = "<root>test content</root>" ;
    XRef<xl::XElement>(xml.root()).addNameSpace("xmlns:f", xl::XMLValue("http://www.w3.org/TR/html4/", "http://www.w3.org/TR/html4/"));
    destination.clear();
    xml.stringify(destination);
    PLOG_INFO << destination.toString();
    XRef<xl::XElement>(xml.root()).addNameSpace("xmlns:h", xl::XMLValue("http://www.w3.org/TR/html5/", "http://www.w3.org/TR/html5/"));
    XRef<xl::XElement>(xml.root()).addNameSpace("xmlns:i", xl::XMLValue("http://www.w3.org/TR/html6/", "http://www.w3.org/TR/html6/"));
    destination.clear();
    xml.stringify(destination);
    PLOG_INFO << destination.toString();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "XML_Create_At_Runtime exited.";
  exit(EXIT_SUCCESS);
}
