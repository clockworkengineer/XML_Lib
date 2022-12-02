#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <unordered_map>
// ===
// XML
// ===
#include "XML_Core.hpp"
// ===========================
// XML Entity Mapper interface
// ===========================
#include "IEntityMapper.hpp"
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// CLASS DEFINITION
// ================
class DTD_Impl
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  explicit DTD_Impl(XDTD &xNodeDTD);
  DTD_Impl() = delete;
  DTD_Impl(const DTD_Impl &other) = delete;
  DTD_Impl &operator=(const DTD_Impl &other) = delete;
  DTD_Impl(DTD_Impl &&other) = delete;
  DTD_Impl &operator=(DTD_Impl &&other) = delete;
  ~DTD_Impl();
  // ==============
  // PUBLIC METHODS
  // ==============
  void parse(ISource &source);
  void stringify(IDestination &destination);
  // ================
  // PUBLIC VARIABLES
  // ================
private:
  // ===========================
  // PRIVATE TYPES AND CONSTANTS
  // ===========================
  // ===========================================
  // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
  // ===========================================
  // ===============
  // PRIVATE METHODS
  // ===============
  void parseValidNotations(const std::string &notations);
  void parseValidateAttribute(const std::string &elementName, const XDTD::Attribute &dtdAttribute);
  void parseConditional(ISource &source, bool includeOff = true);
  void parseParameterEntities(ISource &source);
  void parseExternalContent(ISource &source);
  std::string parseAttributeEnumerationType(ISource &source);
  bool parseIsChoiceOrSequence(ISource &contentSpecSource);
  void parseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementName(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
  XMLValue parseElementContentSpecification(const std::string &elementName, const XMLValue &contentSpec);
  void parseExternalReferenceContent();
  void parseAttributeList(ISource &source);
  void parseNotation(ISource &source);
  void parseEntity(ISource &source);
  void parseElement(ISource &source);
  XMLExternalReference parseExternalReference(ISource &source);
  void parseAttributeType(ISource &source, XDTD::Attribute &attribute);
  void parseAttributeValue(ISource &source, XDTD::Attribute &attribute);
  void parseComment(ISource &source);
  void parseParameterEntityReference(ISource &source);
  void parseExternal(ISource &source);
  void parseInternal(ISource &source);
  void parseDTD(ISource &source);
  // =================
  // PRIVATE VARIABLES
  // =================
  XDTD &m_xNodeDTD;
};
}// namespace XML_Lib
