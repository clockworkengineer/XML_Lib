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
    // add a number that is stored as double (note the implicit conversion of
    // json to an object)
    // xml["pi"] = 3.141;
    // // add a Boolean that is stored as bool
    // xml["sad"] = true;
    // // add a string that is stored as std::string
    // xml["first_name"] = "Niels";
    // // add another null object by passing nullptr
    // xml["nothing"] = nullptr;
    // // add an object inside the object
    // xml["the_answer"]["everything"] = 42;
    // // add an array that is stored as std::vector (using an initializer list)
    // xml["list"] = { 1, 0, 2 };
    // // add another object (using an initializer list of pairs)
    // xml["object"] = { { "currency", "USD" }, { "value", 42.99 } };
    // // add another object that has a nested array (XNode{})
    // xml["object2"] = { { "currency", "USD" }, { "array", xl::XNode{ 23.22, 33, 55, 99.99 } } };
    // xl::BufferDestination destination;
    // xml.stringify(destination);
    // PLOG_INFO << destination.toString();
    // destination.clear();
    // // create XML using an intialiser list and nesting array/objects using XNode{}.
    // xl::XML xml2 = { { "pi", 3.141 },
    //   { "sad", true },
    //   { "first_name", "Niels" },
    //   { "nothing", nullptr },
    //   { "the_answer", xl::XNode{ { "everything", 42 } } },
    //   { "list", xl::XNode{ 1, 0, 2 } },
    //   { "object", xl::XNode{ { "currency", "USD" }, { "value", xl::XNode{ 23.22, 33, 55, 99.99 } } } } };
    // xml2.stringify(destination);
    // PLOG_INFO << destination.toString();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "XML_Create_At_Runtime exited.";
  exit(EXIT_SUCCESS);
}
