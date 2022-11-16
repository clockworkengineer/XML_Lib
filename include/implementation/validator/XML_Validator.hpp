#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <set>
#include <regex>
// ===
// XML
// ===
#include "XML_Types.hpp"
#include "XML_Core.hpp"
#include "XML_Sources.hpp"
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
  explicit XML_Validator(XNodeDTD &xNodeDTD) : m_xNodeDTD(xNodeDTD) {}
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
  void elementError(const XNodeElement &xNodeElement, const std::string &error);
  void checkAttributes(const XNode &xNode);
  void checkContentSpecification(const XNode &xNode);
  void checkElement(const XNode &xNode);
  void checkElements(const XNode &xNode);
  bool checkIsNMTOKENOK(const std::string &nmTokenValue);
  bool checkIsIDOK(const std::string &idValue);
  void checkAttributeValue(const XNode &xNode, const XNodeDTD::Attribute &attribute);
  void checkAttributeType(const XNode &xNode, const XNodeDTD::Attribute &attribute);
  bool checkIsPCDATA(const XNode &xNode);
  bool checkIsEMPTY(const XNode &xNode);
  void checkAgainstDTD(const XNode &prolog);
  // =================
  // PRIVATE VARIABLES
  // =================
  std::set<std::string> m_assignedIDValues;
  std::set<std::string> m_assignedIDREFValues;
  long m_lineNumber = 1;
  XNodeDTD &m_xNodeDTD;
};
}// namespace XML_Lib
