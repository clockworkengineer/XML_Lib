#pragma once

#include "XML_Core.hpp"

#include "DTD.hpp"

namespace XML_Lib {

class DTD_Impl
{
public:
  explicit DTD_Impl(XNode &xNode);
  DTD_Impl() = delete;
  DTD_Impl(const DTD_Impl &other) = delete;
  DTD_Impl &operator=(const DTD_Impl &other) = delete;
  DTD_Impl(DTD_Impl &&other) = delete;
  DTD_Impl &operator=(DTD_Impl &&other) = delete;
  ~DTD_Impl();

  // Parse XML DTD from source
  void parse(ISource &source);
  // Stringify XML DTD to text destination
  void stringify(IDestination &destination);

private:
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

  XDTD &m_xDTD;
};
}// namespace XML_Lib
