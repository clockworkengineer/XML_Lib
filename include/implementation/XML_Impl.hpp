#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Core.hpp"

#include "DTD.hpp"
#include "DTD_Validator.hpp"

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
  // XML Parser
  std::string parseDeclarationAttribute(ISource &source, const std::string &name, const std::set<std::string> &values);
  bool parseCommentsPIAndWhiteSpace(ISource &source, XNode &xProlog);
  void parseContent(ISource &source, XNode &xNode);
  std::string parseTagName(ISource &source);
  std::vector<XMLAttribute> parseAttributes(ISource &source);
  XNode parseComment(ISource &source);
  XNode parseCDATA(ISource &source);
  XNode parsePI(ISource &source);
  void parseWhiteSpaceToContent(ISource &source, XNode &xNode);
  void parseElementInternal(ISource &source, XNode &xNode);
  XNode parseElement(ISource &source, const std::vector<XMLAttribute> &outerNamespaces);
  XNode parseDeclaration(ISource &source);
  XNode parseDTD(ISource &source);
  XNode parseProlog(ISource &source);
  void parseEpilog(ISource &source, XNode &xProlog);
  XNode parseXML(ISource &source);
  // XML Stringification
  void stringifyXML(const XNode &xNode, IDestination &destination);
  void stringifyXML(IDestination &destination);
  // Miscellaneous
  void resetWhiteSpace(XNode &xNode);
  void processEntityReferenceXML(XNode &xNode, const XMLValue &entityReference);
  void addContentToElementChildList(XNode &xNode, const std::string &content);
  void addNewNameSpaces(const std::vector<XMLAttribute> &attributes, std::vector<XMLAttribute> &namespaces);

  bool hasDTD{ false };
  bool hasRoot { false };
  XNode xmlRoot;
  std::unique_ptr<IValidator> validator;
  std::unique_ptr<IEntityMapper> entityMapper;
};
}// namespace XML_Lib
