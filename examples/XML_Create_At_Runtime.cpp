//
// Program: XML_Create_At_Runtime
//
// Description: Create XML programatically at runtime using a combination
// of the XML class, indexing/key operators plus the use of intializer lists.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"
#include "XML.hpp"
#include "XML_Core.hpp"

namespace xl = XML_Lib;
namespace fs = std::filesystem;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  try {
    // Initialise logging.
    plog::init(plog::debug, "XML_Create_At_Runtime.log");
    PLOG_INFO << "XML_Create_At_Runtime started ...";
    // Log version
    PLOG_INFO << xl::XML().version();
    // create an empty structure (null)
    xl::XML xml;
    xl::BufferDestination destination;
    xml.stringify(destination);
    PLOG_INFO << destination.toString();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "XML_Create_At_Runtime exited.";
  exit(EXIT_SUCCESS);
}
