#pragma once
#include "common/XML_Error.hpp"

#include "XML.hpp"
#include "XML_Core.hpp"

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
#if defined(XML_LIB_ENABLE_XSD)
  void validate(const std::string_view &xsdSource);
#endif
#if defined(XML_LIB_ENABLE_XPATH)
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
  // Root Node (children allocated in xmlParser's arena; must be destroyed before xmlParser)
  Node xmlRoot;
  // Traverse XML tree
  template<typename T> static void traverseNodes(T &xNode, IAction &action);
};
/// <summary>
/// Recursively traverse Node tree calling IAction methods and possibly
/// modifying the tree contents or even structure.
/// </summary>
/// <param name="xNode">Node tree to be traversed.</param>
/// <param name="action">Action methods to call during traversal.</param>
template<typename T> void XML_Impl::traverseNodes(T &xNode, IAction &action)
{
  action.onNode(xNode);
  if (isA<Prolog>(xNode)) {
    action.onProlog(xNode);
  } else if (isA<Declaration>(xNode)) {
    action.onDeclaration(xNode);
  } else if (isA<Root>(xNode)) {
    action.onRoot(xNode);
  } else if (isA<Self>(xNode)) {
    action.onSelf(xNode);
  } else if (isA<Element>(xNode)) {
    action.onElement(xNode);
  } else if (isA<Content>(xNode)) {
    action.onContent(xNode);
  } else if (isA<EntityReference>(xNode)) {
    action.onEntityReference(xNode);
  } else if (isA<Comment>(xNode)) {
    action.onComment(xNode);
  } else if (isA<CDATA>(xNode)) {
    action.onCDATA(xNode);
  } else if (isA<PI>(xNode)) {
    action.onPI(xNode);
  } else if (isA<DTD>(xNode)) {
    action.onDTD(xNode);
  } else {
    XML_LIB_THROW(Error("Unknown Node type encountered during tree traversal."));
  }
  if (!xNode.getChildren().empty()) {
    for (auto &child : xNode.getChildren()) { traverseNodes(child, action); }
  }
}
}// namespace XML_Lib
