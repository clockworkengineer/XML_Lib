#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

class Default_Parser final : public IParser
{
public:
  // Constructors/Destructors
  explicit Default_Parser(IEntityMapper &entityMapper) : entityMapper(entityMapper) {}
  Default_Parser(const Default_Parser &other) = delete;
  Default_Parser &operator=(const Default_Parser &other) = delete;
  Default_Parser(Default_Parser &&other) = delete;
  Default_Parser &operator=(Default_Parser &&other) = delete;
  ~Default_Parser() override = default;

  [[nodiscard]] XNode parse(ISource &source) override;
  [[nodiscard]] bool canValidate() override;
  void validate(XNode &xProlog) override;

private:
  // XML Parser
  static void parseEntityReferenceXML(XNode &xNode, const XMLValue &entityReference, IEntityMapper & entityMapper);
  [[nodiscard]] static std::string
    parseDeclarationAttribute(ISource &source, const std::string_view &name, const std::set<std::string> &values);
  [[nodiscard]] static bool parseCommentsPIAndWhiteSpace(ISource &source, XNode &xProlog);
  static void  parseContent(ISource &source, XNode &xNode, IEntityMapper &entityMapper);
  [[nodiscard]] static std::string parseTagName(ISource &source);
  [[nodiscard]] static std::vector<XMLAttribute> parseAttributes(ISource &source, IEntityMapper &entityMapper);
  [[nodiscard]] static XNode parseComment(ISource &source);
  [[nodiscard]] static XNode parseCDATA(ISource &source);
  [[nodiscard]] static XNode parsePI(ISource &source);
  static void parseWhiteSpaceToContent(ISource &source, XNode &xNode);
  static void parseElementInternal(ISource &source, XNode &xNode, IEntityMapper &entityMapper);
  [[nodiscard]] static XNode parseElement(ISource &source, const std::vector<XMLAttribute> &namespaces, IEntityMapper & entityMapper);
  [[nodiscard]] static XNode parseDeclaration(ISource &source);
  [[nodiscard]] static XNode parseDTD(ISource &source, IEntityMapper &entityMapper);
  [[nodiscard]] static XNode parseProlog(ISource &source, IEntityMapper &entityMapper);
  static void parseEpilog(ISource &source, XNode &xProlog);
  // XML tree has root
  inline static bool hasRoot{ false };
  // Parser validator
  inline static std::unique_ptr<IValidator> validator;
  // Entity mapper reference
  IEntityMapper &entityMapper;
};
}// namespace XML_Lib
