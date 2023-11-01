#pragma once
// ========
// XML Core
// ========
#include "XML_Core.hpp"
// ===
// DTD
// ===
#include "DTD.hpp"

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
  explicit DTD_Impl(XNode &xNode);
  DTD_Impl() = delete;
  DTD_Impl(const DTD_Impl &other) = delete;
  DTD_Impl &operator=(const DTD_Impl &other) = delete;
  DTD_Impl(DTD_Impl &&other) = delete;
  DTD_Impl &operator=(DTD_Impl &&other) = delete;
  ~DTD_Impl();
  // ==============
  // PUBLIC METHODS
  // ==============
  // Parse XML DTD from source
  void parse(ISource &source);
  // Stringify XML DTD to text destination
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
  XDTD &m_xDTD;
};
}// namespace XML_Lib
