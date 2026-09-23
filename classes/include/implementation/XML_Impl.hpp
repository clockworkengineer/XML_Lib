#pragma once
#include "common/XML_Error.hpp"

#include "XML.hpp"
#include "XML_Core.hpp"
#include "NodeVisitorAdapter.hpp"

namespace XML_Lib {

class XML_Impl
{
public:
  // Constructors/Destructors
  explicit XML_Impl(IStringify *stringify, IParser *parser);
  XML_Impl(const XML_Impl &other) = delete;
  XML_Impl &operator=(const XML_Impl &other) = delete;
  XML_Impl(XML_Impl &&other) = delete;
  XML_Impl &operator=(XML_Impl &&other) = delete;
  ~XML_Impl();

#if defined(XML_LIB_ENABLE_DTD)
  [[nodiscard]] Node &dtd();
#endif
  [[nodiscard]] Node &prolog();
  [[nodiscard]] Node &root();
  [[nodiscard]] Node &declaration();
  void parse(ISource &source, const ParseOptions &options = {});
#if defined(XML_LIB_ENABLE_STRINGIFY)
  void stringify(IDestination &destination);
#endif
  void traverse(IAction &action);
  void traverse(IAction &action) const;
#if defined(XML_LIB_ENABLE_DTD)
  void validate();
#endif
  void registerValidator(const std::string_view &schemaType, std::unique_ptr<IValidator> validator);
  void validate(const std::string_view &schemaType, const std::string_view &schemaSource);
#if defined(XML_LIB_ENABLE_XSD)
  void validate(const std::string_view &xsdSource);
#endif
#if defined(XML_LIB_ENABLE_XPATH)
  void setXPathEngine(std::unique_ptr<IXPathEngine> engine);
  [[nodiscard]] std::vector<const Node *> xpath(std::string_view expression);
#endif
  [[nodiscard]] static std::string version();

  [[nodiscard]] static std::string fromFile(const std::filesystem::path &filePath);
  static void toFile(const std::filesystem::path &filePath, const std::string_view &xmlString, XML::Format format);
  [[nodiscard]] static XML::Format getFileFormat(const std::string_view &fileName);

private:
  // Embedded arena for parser-allocated nodes (size governed by XML_LIB_ARENA_SIZE_KB)
  XML_Arena arena{ static_cast<std::size_t>(XML_LIB_ARENA_SIZE_KB) * 1024 };
  // Entity mapper
  std::unique_ptr<IEntityMapper> entityMapper;
  // XML stringifier
  std::unique_ptr<IStringify> xmlStringifier;
  // XML parser (owns Default_Parser::arena; must outlive xmlRoot)
  std::unique_ptr<IParser> xmlParser;
  // Validator registry
  std::unique_ptr<IValidatorRegistry> validatorRegistry;
#if defined(XML_LIB_ENABLE_XPATH)
  // XPath engine
  std::unique_ptr<IXPathEngine> xpathEngine;
#endif
  // Root Node (children allocated in xmlParser's arena; must be destroyed before xmlParser)
  Node xmlRoot;
  // Traverse XML tree
  template<typename T, typename VisitorT> static void traverseNodes(T &xNode, VisitorT &action);
};

/// <summary>
/// Recursively traverse Node tree calling visitor methods via NodeVisitorAdapter (ISP compliant).
/// </summary>
template<typename T, typename VisitorT>
void XML_Impl::traverseNodes(T &xNode, VisitorT &action)
{
  NodeVisitorAdapter::dispatchVisit(xNode, action);
  if (!xNode.getChildren().empty()) {
    for (auto &child : xNode.getChildren()) { traverseNodes(child, action); }
  }
}

} // namespace XML_Lib
