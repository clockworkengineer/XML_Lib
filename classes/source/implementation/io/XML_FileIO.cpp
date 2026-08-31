//
// Class: XML_FileIO
//
// Description: Dedicated XML file I/O service implementation.
//
// Dependencies: C++20 - Language standard features used.
//

#include "implementation/io/XML_FileIO.hpp"
#include "implementation/XML_Impl.hpp"

namespace XML_Lib {

std::string XML_FileIO::fromFile(const std::filesystem::path &filePath)
{
  return XML_Impl::fromFile(filePath);
}

void XML_FileIO::toFile(const std::filesystem::path &filePath, const std::string_view &xmlString, XML::Format format)
{
  XML_Impl::toFile(filePath, xmlString, format);
}

XML::Format XML_FileIO::getFileFormat(const std::string_view &fileName)
{
  return XML_Impl::getFileFormat(fileName);
}

} // namespace XML_Lib
