#pragma once

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

  [[nodiscard]] XNode &dtd();
  [[nodiscard]] XNode &prolog();
  [[nodiscard]] XNode &root();
  [[nodiscard]] XNode &declaration();
  void parse(ISource &source);
  void stringify(IDestination &destination);
  void traverse(IAction &action);
  void traverse(IAction &action) const;
  void validate();
  [[nodiscard]] static std::string version();

  [[nodiscard]] static std::string fromFile(const std::string_view &fileName);
  static void toFile(const std::string_view &fileName, const std::string_view &xmlString, XML::Format format);
  [[nodiscard]] static XML::Format getFileFormat(const std::string_view &fileName);

private:
  // Root XNode
  XNode xmlRoot;
  // Traverse JSON tree
  template<typename T> static void traverseXNodes(T &xNode, IAction &action);
  // Entity mapper
  std::unique_ptr<IEntityMapper> entityMapper;
  // XML parser
  std::unique_ptr<IParser> xmlParser;
  // XML stringifier
  std::unique_ptr<IStringify> xmlStringifier;
};
/// <summary>
/// Recursively traverse XNode tree calling IAction methods and possibly
/// modifying the tree contents or even structure.
/// </summary>
/// <param name="xNode">XNode tree to be traversed.</param>
/// <param name="action">Action methods to call during traversal.</param>
template<typename T> void XML_Impl::traverseXNodes(T &xNode, IAction &action)
{
  action.onXNode(xNode);
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
    throw Error("Unknown XNode type encountered during tree traversal.");
  }
  if (!xNode.getChildren().empty()) {
    for (auto &child : xNode.getChildren()) { traverseXNodes(child, action); }
  }
}
}// namespace XML_Lib
