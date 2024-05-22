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
  void parseConditional(ISource &source, bool includeOn = true);
  void parseExternalContent(ISource &source);
  [[nodiscard]] static std::string parseAttributeEnumerationType(ISource &source);
  static bool parseIsChoiceOrSequence(ISource &contentSpecSource);
  void parseElementCP(ISource &contentSpecSource, IDestination &contentSpecDestination);
  void parseElementChoice(ISource &contentSpecSource, IDestination &contentSpecDestination);
  void parseElementSequence(ISource &contentSpecSource, IDestination &contentSpecDestination);
  void parseElementChildren(ISource &contentSpecSource, IDestination &contentSpecDestination);
  static void parseElementName(ISource &contentSpecSource, IDestination &contentSpecDestination);
  static void parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpecDestination);
  [[nodiscard]] XMLValue parseElementInternalpecification(const std::string &elementName, const XMLValue &contentSpec);
  void parseExternalReferenceContent();
  void parseAttributeList(ISource &source) const;
  void parseNotation(ISource &source) const;
  void parseEntity(ISource &source) const;
  void parseElement(ISource &source);
  [[nodiscard]] XMLExternalReference parseExternalReference(ISource &source) const;
  void parseAttributeType(ISource &source, XDTD::Attribute &attribute) const;
  void parseAttributeValue(ISource &source, XDTD::Attribute &attribute) const;
  static void parseComment(ISource &source);
  void parseParameterEntityReference(ISource &source);
  void parseExternal(ISource &source);
  void parseInternal(ISource &source);
  void parseDTD(ISource &source);

  void elementError(const XElement &xElement, const std::string &error) const;
  void checkAttributes(const XNode &xNode);
  void checkContentSpecification(const XNode &xNode) const;
  void checkElement(const XNode &xNode);
  void checkElements(const XNode &xNode);
  [[nodiscard]] static bool checkIsNMTOKENOK(const std::string &nmTokenValue);
  [[nodiscard]] static bool checkIsIDOK(const std::string &idValue);
  void checkAttributeValue(const XNode &xNode, const XDTD::Attribute &attribute) const;
  void checkAttributeType(const XNode &xNode, const XDTD::Attribute &attribute);
  [[nodiscard]] static bool checkIsPCDATA(const XNode &xNode);
  [[nodiscard]] static bool checkIsEMPTY(const XNode &xNode);
  void checkAgainstDTD(const XNode &xNode);

  std::set<std::string> assignedIDValues;
  std::set<std::string> assignedIDREFValues;
  long lineNumber = 1;
  XDTD &xDTD;
};
}// namespace XML_Lib
