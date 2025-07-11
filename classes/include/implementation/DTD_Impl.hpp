#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"
#include "DTD_Validator.hpp"

namespace XML_Lib {

class DTD_Impl
{
public:
  // Constructors/Destructors
  explicit DTD_Impl(Node &xNode);
  DTD_Impl() = delete;
  DTD_Impl(const DTD_Impl &other) = delete;
  DTD_Impl &operator=(const DTD_Impl &other) = delete;
  DTD_Impl(DTD_Impl &&other) = delete;
  DTD_Impl &operator=(DTD_Impl &&other) = delete;
  ~DTD_Impl();

  // Parse XML DTD_Validator from source
  void parse(ISource &source);
  // Stringify XML DTD_Validator to text destination
  void stringify(IDestination &destination) const;
  // Validate XML against DTD_Validator
  void validate(const Node &xNode);

private:
  void parseValidNotations(const std::string_view &notations) const;
  void parseValidateAttribute(const std::string_view &elementName, const DTD::Attribute &dtdAttribute) const;
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
  [[nodiscard]] XMLValue parseElementInternalSpecification(const std::string_view &elementName, const XMLValue &contentSpec);
  void parseExternalReferenceContent();
  void parseAttributeList(ISource &source) const;
  void parseNotation(ISource &source) const;
  void parseEntity(ISource &source) const;
  void parseElement(ISource &source);
  [[nodiscard]] XMLExternalReference parseExternalReference(ISource &source) const;
  void parseAttributeType(ISource &source, DTD::Attribute &attribute) const;
  void parseAttributeValue(ISource &source, DTD::Attribute &attribute) const;
  static void parseComment(ISource &source);
  void parseParameterEntityReference(ISource &source);
  void parseExternal(ISource &source);
  void parseInternal(ISource &source);
  void parseDTD(ISource &source);

  void elementError(const Element &xElement, const std::string_view &error) const;
  void checkAttributes(const Node &xNode);
  void checkContentSpecification(const Node &xNode) const;
  void checkElement(const Node &xNode);
  void checkElements(const Node &xNode);
  [[nodiscard]] static bool checkIsNMTOKENOK(const std::string_view &nmTokenValue);
  [[nodiscard]] static bool checkIsIDOK(const std::string_view &idValue);
  void checkAttributeValue(const Node &xNode, const DTD::Attribute &attribute) const;
  void checkAttributeType(const Node &xNode, const DTD::Attribute &attribute);
  [[nodiscard]] static bool checkIsPCDATA(const Node &xNode);
  [[nodiscard]] static bool checkIsEMPTY(const Node &xNode);
  void checkAgainstDTD(const Node &xNode);

  std::set<std::string> assignedIDValues;
  std::set<std::string> assignedIDREFValues;
  long lineNumber = 1;
  DTD &xDTD;
};
}// namespace XML_Lib
