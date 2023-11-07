#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Config.hpp"
#include "XML_Core.hpp"
#include "DTD_Validator.hpp"
#include "XML_EntityMapper.hpp"

#include "DTD.hpp"

namespace XML_Lib {

class XML_Impl
{
public:
  // Constructors/Destructors
  XML_Impl();
  XML_Impl(const XML_Impl &other) = delete;
  XML_Impl &operator=(const XML_Impl &other) = delete;
  XML_Impl(XML_Impl &&other) = delete;
  XML_Impl &operator=(XML_Impl &&other) = delete;
  ~XML_Impl();

  XNode &dtd();
  XNode &prolog();
  XNode &root();
  XNode &declaration();
  void parse(ISource &source);
  void stringify(IDestination &destination);
  void validate();
  std::string version();

private:
  void resetWhiteSpace(XNode &xNode);
  void processEntityReferenceXML(XNode &xNode, const XMLValue &entityReference);
  void addContentToElementChildList(XNode &xNode, const std::string &content);
  std::string parseDeclarationAttribute(ISource &source,
    const std::string &name,
    const std::set<std::string> &values,
    bool toUpper);
  void parseElementContent(ISource &source, XNode &xNode);
  std::string parseTagName(ISource &source);
  std::vector<XMLAttribute> parseAttributes(ISource &source);
  std::unique_ptr<XNode> parseComment(ISource &source);
  std::unique_ptr<XNode> parseCDATA(ISource &source);
  std::unique_ptr<XNode> parsePI(ISource &source);
  void parseWhiteSpaceToContent(ISource &source, XNode &xNode);
  void parseElementContents(ISource &source, XNode &xNode);
  std::unique_ptr<XNode> parseElement(ISource &source,
    const std::vector<XMLAttribute> &outerNamespaces,
    XNode::Type xNodeType = XNode::Type::element);
  std::unique_ptr<XNode> parseDeclaration(ISource &source);
  std::unique_ptr<XNode> parseDTD(ISource &source);
  std::unique_ptr<XNode> parseProlog(ISource &source);
  void parseTail(ISource &source, XNode &xProlog);
  std::unique_ptr<XNode> parseXML(ISource &source);
  void stringifyElements(XNode &xNode, IDestination &destination);
  void stringifyXML(IDestination &destination);

  std::unique_ptr<XNode> xmlProlog;
  std::unique_ptr<IValidator> validator;
  std::unique_ptr<IEntityMapper> entityMapper;
};
}// namespace XML_Lib
