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

  [[nodiscard]] Node parse(ISource &source) override;
  [[nodiscard]] bool canValidate() override;
  void validate(Node &xProlog) override;

private:
  // XML Parser
  static void parseEntityReferenceXML(Node &xNode, const XMLValue &entityReference, IEntityMapper & entityMapper);
  [[nodiscard]] static std::string
    parseDeclarationAttribute(ISource &source, const std::string_view &name, const std::set<std::string> &values);
  [[nodiscard]] static bool parseCommentsPIAndWhiteSpace(ISource &source, Node &xProlog);
  static void  parseContent(ISource &source, Node &xNode, IEntityMapper &entityMapper);
  [[nodiscard]] static std::string parseTagName(ISource &source);
  [[nodiscard]] static std::vector<XMLAttribute> parseAttributes(ISource &source, IEntityMapper &entityMapper);
  [[nodiscard]] static Node parseComment(ISource &source);
  [[nodiscard]] static Node parseCDATA(ISource &source);
  [[nodiscard]] static Node parsePI(ISource &source);
  static void parseWhiteSpaceToContent(ISource &source, Node &xNode);
  static void parseElementInternal(ISource &source, Node &xNode, IEntityMapper &entityMapper);
  [[nodiscard]] static Node parseElement(ISource &source, const std::vector<XMLAttribute> &namespaces, IEntityMapper & entityMapper);
  [[nodiscard]] static Node parseDeclaration(ISource &source);
  [[nodiscard]] static Node parseDTD(ISource &source, IEntityMapper &entityMapper);
  [[nodiscard]] static Node parseProlog(ISource &source, IEntityMapper &entityMapper);
  static void parseEpilog(ISource &source, Node &xProlog);
  // XML tree has root
  inline static bool hasRoot{ false };
  // Parser validator
  inline static std::unique_ptr<IValidator> validator;
  // Entity mapper reference
  IEntityMapper &entityMapper;
};
}// namespace XML_Lib
