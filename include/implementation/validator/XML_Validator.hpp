#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <set>
#include <regex>
// ========
// XML Core
// ========
#include "XML_Core.hpp"
// ========================================
// XML Validator / Entity Mapper interfaces
// ========================================
#include "IValidator.hpp"
#include "IEntityMapper.hpp"
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// CLASS DEFINITION
// ================
class XML_Validator : public IValidator
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  explicit XML_Validator(XNode &xNode) : m_xNodeDTD(XRef<XDTD>(xNode)) {}
  XML_Validator(const XML_Validator &other) = delete;
  XML_Validator &operator=(const XML_Validator &other) = delete;
  XML_Validator(XML_Validator &&other) = delete;
  XML_Validator &operator=(XML_Validator &&other) = delete;
  ~XML_Validator() override = default;
  // ==========
  // DESTRUCTOR
  // ==========
  // ==============
  // PUBLIC METHODS
  // ==============
  void validate(const XNode &prolog) override;
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
  // =================
  // PRIVATE VARIABLES
  // =================
  std::set<std::string> m_assignedIDValues;
  std::set<std::string> m_assignedIDREFValues;
  long m_lineNumber = 1;
  XDTD &m_xNodeDTD;
};
}// namespace XML_Lib
