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
  XMLNode &dtd();
  XMLNode &prolog();
  XMLNode &root();
  XMLNode &declaration();
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
  void resetWhiteSpace(XMLNode &xmlNode);
  void processEntityReferenceXML(XMLNode &xmlNode, const XMLValue &entityReference);
  void addContentToElementChildList(XMLNode &xmlNode, const std::string &content);
  void parseElementContent(ISource &source, XMLNode &xmlNode);
  std::string parseTagName(ISource &source);
  XMLAttribute::List parseAttributes(ISource &source);
  XMLNode::Ptr parseComment(ISource &source);
  XMLNode::Ptr parseCDATA(ISource &source);
  XMLNode::Ptr parsePI(ISource &source);
  void parseWhiteSpaceToContent(ISource &source, XMLNode &xmlNode);
  void parseElementContents(ISource &source, XMLNode &xmlNode);
  XMLNode::Ptr
    parseElement(ISource &source, const XMLAttribute::List &namespaces, XMLNodeType xmlNodeType = XMLNodeType::element);
  XMLNode::Ptr parseDeclaration(ISource &source);
  XMLNode::Ptr parseDTD(ISource &source);
  XMLNode::Ptr parseProlog(ISource &source);
  void parseXMLTail(ISource &source);
  void parseXML(ISource &source);
  void stringifyElements(XMLNode &xmlNode, IDestination &destination);
  void stringifyXML(IDestination &destination);
  // =================
  // PRIVATE VARIABLES
  // =================
  XMLNode::Ptr m_prolog;
  std::unique_ptr<DTD> m_dtd;
  std::unique_ptr<IValidator> m_validator;
  std::unique_ptr<IEntityMapper> m_entityMapper;
};
}// namespace XML_Lib
