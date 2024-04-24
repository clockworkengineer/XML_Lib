#pragma once

#include <memory>
#include <string>
#include <stdexcept>

#include "XML_Config.hpp"

namespace XML_Lib {

// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class XML_Impl;
class ISource;
class IDestination;
struct XNode;

class XML
{
public:
  // XML file formats
  enum class Format : uint8_t { utf8 = 0, utf8BOM, utf16BE, utf16LE, utf32BE, utf32LE };
  // Constructors/Destructors
  XML();
  XML(const XML &other) = delete;
  XML &operator=(const XML &other) = delete;
  XML(XML &&other) = delete;
  XML &operator=(XML &&other) = delete;
  ~XML();
  // Get reference to DTD XNode
  XNode &dtd();
  // Get reference to prolog XNode
  XNode &prolog();
  // Get reference to declaration XNode
  XNode &declaration();
  // Get reference to root element XNode
  XNode &root();
  // Validate XML
  void validate();
  // Return version string
  std::string version();
  // Parse XML source
  void parse(ISource &source);
  void parse(ISource &&source);
  // Stringify XML object to destination text
  void stringify(IDestination &destination);
  void stringify(IDestination &&destination);
  // Read/Write XML to/from file
  static const std::string fromFile(const std::string &fileName);
  static void toFile(const std::string &fileName, const std::string &xmlString, Format format = Format::utf8);
  // Get XML file format
  static Format getFileFormat(const std::string &fileName);

private:
  const std::unique_ptr<XML_Impl> implementation;
};
}// namespace XML_Lib
