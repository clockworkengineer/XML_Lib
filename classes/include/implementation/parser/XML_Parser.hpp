#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

class XML_Parser final : public IParser
{
public:
  // Constructors/Destructors
  explicit XML_Parser(IEntityMapper &entityMapper) : entityMapper(entityMapper) {}
  XML_Parser(const XML_Parser &other) = delete;
  XML_Parser &operator=(const XML_Parser &other) = delete;
  XML_Parser(XML_Parser &&other) = delete;
  XML_Parser &operator=(XML_Parser &&other) = delete;
  ~XML_Parser() override = default;

  [[nodiscard]] XNode parse(ISource &source) override;
  [[nodiscard]] bool canValidate() override;
  void validate(XNode &xProlog) override;

private:
  // XML Parser
  void parseEntityReferenceXML(XNode &xNode, const XMLValue &entityReference);
  [[nodiscard]] static std::string
    parseDeclarationAttribute(ISource &source, const std::string &name, const std::set<std::string> &values);
  [[nodiscard]] static bool parseCommentsPIAndWhiteSpace(ISource &source, XNode &xProlog);
  void parseContent(ISource &source, XNode &xNode);
  [[nodiscard]] static std::string parseTagName(ISource &source);
  [[nodiscard]] std::vector<XMLAttribute> parseAttributes(ISource &source) const;
  [[nodiscard]] static XNode parseComment(ISource &source);
  [[nodiscard]] static XNode parseCDATA(ISource &source);
  [[nodiscard]] static XNode parsePI(ISource &source);
  static void parseWhiteSpaceToContent(ISource &source, XNode &xNode);
  void parseElementInternal(ISource &source, XNode &xNode);
  [[nodiscard]] XNode parseElement(ISource &source, const std::vector<XMLAttribute> &outerNamespaces);
  [[nodiscard]] static XNode parseDeclaration(ISource &source);
  [[nodiscard]] static XNode parseDTD(ISource &source, IEntityMapper &entityMapper);
  [[nodiscard]] static XNode parseProlog(ISource &source, IEntityMapper &entityMapper);
  static void parseEpilog(ISource &source, XNode &xProlog);
  // XML tree has root
  bool hasRoot{ false };
  // Parser validator
  inline static std::unique_ptr<IValidator> validator;
  // Entity mapper reference
  IEntityMapper &entityMapper;
};
}// namespace XML_Lib
