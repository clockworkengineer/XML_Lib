#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <stdexcept>
#include <memory>
// ===
// XML
// ===
#include "XML_Config.hpp"
#include "XML_Types.hpp"
#include "XML_Core.hpp"
#include "XML_Validator.hpp"
#include "XML_EntityMapper.hpp"
#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"
// ===
// DTD
// ===
#include "DTD.hpp"
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// CLASS DEFINITION
// ================
class XML_Impl
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  XML_Impl();
  XML_Impl(const XML_Impl &other) = delete;
  XML_Impl &operator=(const XML_Impl &other) = delete;
  XML_Impl(XML_Impl &&other) = delete;
  XML_Impl &operator=(XML_Impl &&other) = delete;
  ~XML_Impl();
  // ==============
  // PUBLIC METHODS
  // ==============
  XNode &dtd();
  XNode &prolog();
  XNode &root();
  XNode &declaration();
  void parse(ISource &source);
  void stringify(IDestination &destination);
  void validate();
  std::string version();
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
  void resetWhiteSpace(XNode &xNode);
  void processEntityReferenceXML(XNode &xNode, const XMLValue &entityReference);
  void addContentToElementChildList(XNode &xNode, const std::string &content);
  void parseElementContent(ISource &source, XNode &xNode);
  std::string parseTagName(ISource &source);
  XMLAttribute::List parseAttributes(ISource &source);
  XNode::Ptr parseComment(ISource &source);
  XNode::Ptr parseCDATA(ISource &source);
  XNode::Ptr parsePI(ISource &source);
  void parseWhiteSpaceToContent(ISource &source, XNode &xNode);
  void parseElementContents(ISource &source, XNode &xNode);
  XNode::Ptr
    parseElement(ISource &source, const XMLAttribute::List &namespaces, XNodeType xNodeType = XNodeType::element);
  XNode::Ptr parseDeclaration(ISource &source);
  XNode::Ptr parseDTD(ISource &source);
  XNode::Ptr parseProlog(ISource &source);
  void parseXMLTail(ISource &source);
  void parseXML(ISource &source);
  void stringifyElements(XNode &xNode, IDestination &destination);
  void stringifyXML(IDestination &destination);
  // =================
  // PRIVATE VARIABLES
  // =================
  XNode::Ptr m_prolog;
  std::unique_ptr<DTD> m_dtd;
  std::unique_ptr<IValidator> m_validator;
  std::unique_ptr<IEntityMapper> m_entityMapper;
};
}// namespace XML_Lib
