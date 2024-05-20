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
  explicit XML_Parser(IEntityMapper &entityMapper) : entityMapper(entityMapper) {}
  XML_Parser(const XML_Parser &other) = delete;
  XML_Parser &operator=(const XML_Parser &other) = delete;
  XML_Parser(XML_Parser &&other) = delete;
  XML_Parser &operator=(XML_Parser &&other) = delete;
  ~XML_Parser() = default;

  [[nodiscard]] virtual XNode parse(ISource &source);
  [[nodiscard]] virtual bool canValidate();
  virtual void validate(XNode &prolog);

private:
  // XML Parser
  void parseEntityReferenceXML(XNode &xNode, const XMLValue &entityReference);
  [[nodiscard]] std::string
    parseDeclarationAttribute(ISource &source, const std::string &name, const std::set<std::string> &values);
  [[nodiscard]] bool parseCommentsPIAndWhiteSpace(ISource &source, XNode &xProlog);
  void parseContent(ISource &source, XNode &xNode);
  [[nodiscard]] std::string parseTagName(ISource &source);
  [[nodiscard]] std::vector<XMLAttribute> parseAttributes(ISource &source) const;
  [[nodiscard]] XNode parseComment(ISource &source);
  [[nodiscard]] XNode parseCDATA(ISource &source);
  [[nodiscard]] XNode parsePI(ISource &source);
  void parseWhiteSpaceToContent(ISource &source, XNode &xNode);
  void parseElementInternal(ISource &source, XNode &xNode);
  [[nodiscard]] XNode parseElement(ISource &source, const std::vector<XMLAttribute> &outerNamespaces);
  [[nodiscard]] XNode parseDeclaration(ISource &source);
  [[nodiscard]] XNode parseDTD(ISource &source);
  [[nodiscard]] XNode parseProlog(ISource &source);
  void parseEpilog(ISource &source, XNode &xProlog);
  // XML tree has root
  bool hasRoot{ false };
  // Parser validator
  std::unique_ptr<IValidator> validator;
  // Entity mapper reference
  IEntityMapper &entityMapper;
};
}// namespace XML_Lib
