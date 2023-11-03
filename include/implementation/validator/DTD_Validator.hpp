#pragma once

#include <string>
#include <set>
#include <regex>

#include "XML_Core.hpp"

#include "IValidator.hpp"

namespace XML_Lib {

class DTD_Validator : public IValidator
{
public:
  explicit DTD_Validator(XNode &xNode) : xDTD(XRef<XDTD>(xNode)) {}
  DTD_Validator(const DTD_Validator &other) = delete;
  DTD_Validator &operator=(const DTD_Validator &other) = delete;
  DTD_Validator(DTD_Validator &&other) = delete;
  DTD_Validator &operator=(DTD_Validator &&other) = delete;
  ~DTD_Validator() override = default;

  void validate(const XNode &prolog) override;

private:
  void elementError(const XElement &xElement, const std::string &error);
  void checkAttributes(const XNode &xNode);
  void checkContentSpecification(const XNode &xNode);
  void checkElement(const XNode &xNode);
  void checkElements(const XNode &xNode);
  bool checkIsNMTOKENOK(const std::string &nmTokenValue);
  bool checkIsIDOK(const std::string &idValue);
  void checkAttributeValue(const XNode &xNode, const XDTD::Attribute &attribute);
  void checkAttributeType(const XNode &xNode, const XDTD::Attribute &attribute);
  bool checkIsPCDATA(const XNode &xNode);
  bool checkIsEMPTY(const XNode &xNode);
  void checkAgainstDTD(const XNode &prolog);

  std::set<std::string> assignedIDValues;
  std::set<std::string> assignedIDREFValues;
  long lineNumber = 1;
  XDTD &xDTD;
};
}// namespace XML_Lib
