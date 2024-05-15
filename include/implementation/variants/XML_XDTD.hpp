#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>

namespace XML_Lib {
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class IEntityMapper;

struct XDTD : Variant
{
  //
  // DTD Type
  //
  enum Type : uint8_t { internal = (0x1 << 0), external = (0x1 << 1) };
  //
  // XML DTD attribute types. Note only one type bit and
  // value bit should be set for each attribute and that
  // some combinations like id and fixed are illegal and
  // are reported as so by the parser.
  //
  enum AttributeType : uint16_t {
    // Types bits 0 - 9
    cdata = (0x1 << 0),
    enumeration = (0x1 << 1),
    id = (0x1 << 2),
    idref = (0x1 << 3),
    idrefs = (0x1 << 4),
    nmtoken = (0x1 << 5),
    nmtokens = (0x1 << 6),
    entity = (0x1 << 7),
    entities = (0x1 << 8),
    notation = (0x1 << 9),
    // Value bits 10 - 13
    normal = (0x1 << 10),
    required = (0x1 << 11),
    implied = (0x1 << 12),
    fixed = (0x1 << 13)
  };
  //
  // XML DTD attribute definition
  //
  struct Attribute
  {
    std::string name;
    uint16_t type;
    std::string enumeration;
    XMLValue value{ "", "" };
  };
  //
  // XML DTD element definition
  //
  struct Element
  {
    Element() = default;
    Element(std::string name, XMLValue content) : name(std::move(name)), content(std::move(content)) {}
    std::string name;
    bool idAttributePresent = false;
    XMLValue content{ "", "" };
    std::vector<Attribute> attributes;
  };
  // Constructors/Destructors
  explicit XDTD(IEntityMapper &entityMapper) : Variant(Variant::Type::dtd), entityMapper(entityMapper) {}
  XDTD(const XDTD &other) = delete;
  XDTD &operator=(const XDTD &other) = delete;
  XDTD(XDTD &&other) = default;
  XDTD &operator=(XDTD &&other) = delete;
  ~XDTD() = default;
  [[nodiscard]] std::string unparsed() const { return (unparsedDTD); }
  void setUnparsed(const std::string &unparsed) { unparsedDTD = unparsed; }
  [[nodiscard]] uint16_t getType() const { return (dtdNodeType); }
  void setType(uint16_t type) { dtdNodeType = type; }
  [[nodiscard]] std::string getRootName() const { return (dtdNodeName); }
  void setRootName(const std::string &name) { dtdNodeName = name; }
  [[nodiscard]] XMLExternalReference getExternalReference() const { return (externalReference); }
  void setExternalReference(const XMLExternalReference &reference) { externalReference = reference; }
  [[nodiscard]] bool isElementPresent(const std::string &elementName) const
  {
    return (elements.find(elementName) != elements.end());
  }
  [[nodiscard]] XDTD::Element &getElement(const std::string &elementName)
  {
    auto element = elements.find(elementName);
    if (element != elements.end()) { return (element->second); }
    throw XNode::Error("Could not find notation name.");
  }
  void addElement(const std::string &elementName, const XDTD::Element &element)
  {
    elements.emplace(elementName, element);
  }
  [[nodiscard]] long getElementCount() const { return (static_cast<long>(elements.size())); }
  [[nodiscard]] XMLExternalReference &getNotation(const std::string &notationName)
  {
    auto notation = notations.find(notationName);
    if (notation != notations.end()) { return (notation->second); }
    throw XNode::Error("Could not find notation name.");
  }
  void addNotation(const std::string &notationName, const XMLExternalReference &notation)
  {
    notations.emplace(notationName, notation);
  }
  [[nodiscard]] long getNotationCount(const std::string &notationName) const
  {
    return (static_cast<long>(notations.count(notationName)));
  }
  [[nodiscard]] long getLineCount() const { return (lineCount); }
  void setLineCount(long newLineCount) { lineCount = newLineCount; }

  [[nodiscard]] IEntityMapper &getEntityMapper() const { return (entityMapper); }


private:
  uint16_t dtdNodeType{};
  long lineCount{};
  std::string dtdNodeName;
  XMLExternalReference externalReference{ "" };
  std::unordered_map<std::string, XDTD::Element> elements;
  std::unordered_map<std::string, XMLExternalReference> notations;
  std::string unparsedDTD;
  IEntityMapper &entityMapper;
};
}// namespace XML_Lib