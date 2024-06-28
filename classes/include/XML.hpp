#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <filesystem>
#include <set>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <cwctype>
#include <cstring>


#include "XML_Config.hpp"
#include "XML_Interfaces.hpp"

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class XML_Impl;
struct XNode;

class XML
{
public:
  // XML file formats
  enum class Format : uint8_t { utf8 = 0, utf8BOM, utf16BE, utf16LE, utf32BE, utf32LE };
  // Constructors/Destructors
  explicit XML(IStringify *stringify=nullptr, IParser *parser= nullptr);
  // Pass in default XML to parse
  explicit XML(const std::string &xmlString);
  XML(const XML &other) = delete;
  XML &operator=(const XML &other) = delete;
  XML &operator=(const std::string &xmlString);
  XML(XML &&other) = delete;
  XML &operator=(XML &&other) = delete;
  ~XML();
  // Get reference to DTD XNode
  [[nodiscard]] XNode &dtd() const;
  // Get reference to prolog XNode
  [[nodiscard]] XNode &prolog() const;
  // Get reference to declaration XNode
  [[nodiscard]] XNode &declaration() const;
  // Get reference to root element XNode
  [[nodiscard]] XNode &root() const;
  // Validate XML
  void validate() const;
  // Return version string
  [[nodiscard]] std::string version() const;
  // Parse XML source
  void parse(ISource &source) const;
  void parse(ISource &&source) const;
  // Stringify XML object to destination text
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;
  // Traverse XML tree
  void traverse(IAction &action);
  void traverse(IAction &action) const;
  // Read/Write XML to/from file
  [[nodiscard]] static std::string fromFile(const std::string &fileName);
  static void toFile(const std::string &fileName, const std::string &xmlString, Format format = Format::utf8);
  // Get XML file format
  [[nodiscard]] static Format getFileFormat(const std::string &fileName);

private:
  const std::unique_ptr<XML_Impl> implementation;
};
}// namespace XML_Lib
