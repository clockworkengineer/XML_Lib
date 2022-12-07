#pragma once
// =======
// C++ STL
// =======
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class IEntityMapper;
// ===
// DTD
// ===
struct XDTD : XNode
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
  explicit XDTD(IEntityMapper &entityMapper)
    : XNode(XNode::Type::dtd), m_entityMapper(entityMapper)
  {}
  XDTD(const XDTD &other) = delete;
  XDTD &operator=(XDTD &other) = delete;
  XDTD(XDTD &&other) = delete;
  XDTD &operator=(XDTD &&other) = delete;
  ~XDTD() = default;
  [[nodiscard]] std::string unparsed() const { return (m_unparsed); }
  void setUnparsed(const std::string &unparsed) { m_unparsed = unparsed; }
  [[nodiscard]] uint16_t getType() const { return (m_type); }
  void setType(uint16_t type) { m_type = type; }
  [[nodiscard]] std::string getRootName() const { return (m_name); }
  void setRootName(const std::string &name) { m_name = name; }
  [[nodiscard]] XMLExternalReference getExternalReference() const { return (m_external); }
  void setExternalReference(const XMLExternalReference &reference) { m_external = reference; }
  [[nodiscard]] bool isElementPresent(const std::string &elementName) const
  {
    return (m_elements.find(elementName) != m_elements.end());
  }
  XDTD::Element &getElement(const std::string &elementName)
  {
    auto element = m_elements.find(elementName);
    if (element != m_elements.end()) { return (element->second); }
    throw XNode::Error("Could not find notation name.");
  }
  void addElement(const std::string &elementName, const XDTD::Element &element)
  {
    m_elements.emplace(elementName, element);
  }
  [[nodiscard]] long getElementCount() const { return (static_cast<long>(m_elements.size())); }
  XMLExternalReference &getNotation(const std::string &notationName)
  {
    auto notation = m_notations.find(notationName);
    if (notation != m_notations.end()) { return (notation->second); }
    throw XNode::Error("Could not find notation name.");
  }
  void addNotation(const std::string &notationName, const XMLExternalReference &notation)
  {
    m_notations.emplace(notationName, notation);
  }
  [[nodiscard]] long getNotationCount(const std::string &notationName) const
  {
    return (static_cast<long>(m_notations.count(notationName)));
  }
  [[nodiscard]] long getLineCount() const { return (m_lineCount); }
  void setLineCount(long lineCount) { m_lineCount = lineCount; }
  IEntityMapper &m_entityMapper;

private:
  uint16_t m_type{};
  long m_lineCount{};
  std::string m_name;
  XMLExternalReference m_external{ "" };
  std::unordered_map<std::string, XDTD::Element> m_elements;
  std::unordered_map<std::string, XMLExternalReference> m_notations;
  std::string m_unparsed;
};
}// namespace XML_Lib