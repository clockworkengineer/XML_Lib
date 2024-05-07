#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

class XML_Parser : public IParser
{
public:
  // Constructors/Destructors
  XML_Parser(IEntityMapper &entityMapper) : entityMapper(entityMapper) {}
  XML_Parser(const XML_Parser &other) = delete;
  XML_Parser &operator=(const XML_Parser &other) = delete;
  XML_Parser(XML_Parser &&other) = delete;
  XML_Parser &operator=(XML_Parser &&other) = delete;
  ~XML_Parser() = default;

  virtual XNode parse(ISource &source);
  virtual bool canValidate();
  virtual void validate(XNode &prolog);

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
  // XML tree has root
  bool hasRoot{ false };
  // Parser validator
  std::unique_ptr<IValidator> validator;
  // Entity mapper reference
  IEntityMapper &entityMapper;
};
}// namespace XML_Lib
