#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Core.hpp"

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

  static const std::string fromFile(const std::string &fileName);
  static void toFile(const std::string &fileName, const std::string &xmlString, XML::Format format);
  static XML::Format getFileFormat(const std::string &fileName);

private:
  // XML Parser
  void parseEntityReferenceXML(XNode &xNode, const XMLValue &entityReference);
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
  // XML Stringification
  void stringify(const XNode &xNode, IDestination &destination);

  bool hasRoot{ false };
  XNode xmlRoot;
  std::unique_ptr<IValidator> validator;
  std::unique_ptr<IEntityMapper> entityMapper;
};
}// namespace XML_Lib
