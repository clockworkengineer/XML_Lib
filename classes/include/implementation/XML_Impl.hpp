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

  [[nodiscard]] static std::string fromFile(const std::string &fileName);
  static void toFile(const std::string &fileName, const std::string &xmlString, XML::Format format);
  [[nodiscard]] static XML::Format getFileFormat(const std::string &fileName);

private:
  // Root XNode
  XNode xmlRoot;
  // Traverse JSON tree
  template<typename T> static void traverseXNodes(T &xNode, IAction &action);
  // Entity mapper
  std::unique_ptr<IEntityMapper> entityMapper;
  // XML parser
  std::unique_ptr<IParser> parser;
  // XML stringifier
  std::unique_ptr<IStringify> stringifier;
};
/// <summary>
/// Recursively traverse JNode tree calling IAction methods and possibly
/// modifying the tree contents or even structure.
/// </summary>
/// <param name="xNode">XNode tree to be traversed.</param>
/// <param name="action">Action methods to call during traversal.</param>
template<typename T> void XML_Impl::traverseXNodes(T &xNode, IAction &action)
{
  action.onXNode(xNode);
  if (xNode.isProlog()) {
    action.onProlog(xNode);
  } else if (xNode.isDeclaration()) {
    action.onDeclaration(xNode);
  } else if (xNode.isRoot()) {
    action.onRoot(xNode);
  } else if (xNode.isSelf()) {
    action.onSelf(xNode);
  } else if (xNode.isElement()) {
    action.onElement(xNode);
  } else if (xNode.isContent()) {
    action.onContent(xNode);
  } else if (xNode.isEntity()) {
    action.onEntityReference(xNode);
  } else if (xNode.isComment()) {
    action.onComment(xNode);
  } else if (xNode.isCDATA()) {
    action.onCDATA(xNode);
  } else if (xNode.isPI()) {
    action.onPI(xNode);
  } else if (xNode.isDTD()) {
    action.onDTD(xNode);
  } else {
    throw Error("Unknown XNode type encountered during tree traversal.");
  }
}
}// namespace XML_Lib
