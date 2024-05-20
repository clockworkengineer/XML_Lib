#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"
#include "DTD.hpp"

namespace XML_Lib {

class DTD_Impl
{
public:
  // Constructors/Destructors
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
  void stringify(IDestination &destination) const;
  // Validate XML against DTD
  void validate(const XNode &xNode);

private:
  void parseValidNotations(const std::string &notations) const;
  void parseValidateAttribute(const std::string &elementName, const XDTD::Attribute &dtdAttribute) const;
  void parseConditional(ISource &source, bool includeOff = true);
  void parseExternalContent(ISource &source);
  [[nodiscard]] static std::string parseAttributeEnumerationType(ISource &source);
  static bool parseIsChoiceOrSequence(ISource &contentSpecSource);
  void parseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementName(ISource &contentSpecSource, IDestination &contentSpec);
  void parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
  [[nodiscard]] XMLValue parseElementInternalpecification(const std::string &elementName, const XMLValue &contentSpec);
  void parseExternalReferenceContent();
  void parseAttributeList(ISource &source);
  void parseNotation(ISource &source);
  void parseEntity(ISource &source);
  void parseElement(ISource &source);
  [[nodiscard]] XMLExternalReference parseExternalReference(ISource &source) const;
  void parseAttributeType(ISource &source, XDTD::Attribute &attribute) const;
  void parseAttributeValue(ISource &source, XDTD::Attribute &attribute) const;
  void parseComment(ISource &source);
  void parseParameterEntityReference(ISource &source);
  void parseExternal(ISource &source);
  void parseInternal(ISource &source);
  void parseDTD(ISource &source);

  void elementError(const XElement &xElement, const std::string &error) const;
  void checkAttributes(const XNode &xNode);
  void checkContentSpecification(const XNode &xNode);
  void checkElement(const XNode &xNode);
  void checkElements(const XNode &xNode);
  [[nodiscard]] static bool checkIsNMTOKENOK(const std::string &nmTokenValue);
  [[nodiscard]] bool checkIsIDOK(const std::string &idValue);
  void checkAttributeValue(const XNode &xNode, const XDTD::Attribute &attribute) const;
  void checkAttributeType(const XNode &xNode, const XDTD::Attribute &attribute);
  [[nodiscard]] bool checkIsPCDATA(const XNode &xNode);
  [[nodiscard]] bool checkIsEMPTY(const XNode &xNode);
  void checkAgainstDTD(const XNode &prolog);

  std::set<std::string> assignedIDValues;
  std::set<std::string> assignedIDREFValues;
  long lineNumber = 1;
  XDTD &xDTD;
};
}// namespace XML_Lib
