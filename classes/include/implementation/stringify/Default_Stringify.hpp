#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

class Default_Stringify final : public IStringify
{
public:
  // Constructors/Destructors
  Default_Stringify() = default;
  Default_Stringify(const Default_Stringify &other) = delete;
  Default_Stringify &operator=(const Default_Stringify &other) = delete;
  Default_Stringify(Default_Stringify &&other) = delete;
  Default_Stringify &operator=(Default_Stringify &&other) = delete;
  ~Default_Stringify() override = default;

/// <summary>
/// Recursively parse Node passed in to produce XML output on a
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="xNode">Node to convert into XML.</param>
/// <param name="destination">XML destination stream.</param>
/// <param name="indent">Current indentation.</param>
void stringify(const Node &xNode, IDestination &destination, const unsigned long indent) const override
  {
 stringifyNodes(xNode, destination, indent);
}
private:
  static void stringifyNodes(const Node &xNode, IDestination &destination, const unsigned long indent)
  {
  // XML prolog
  if (isA<Prolog>(xNode)) {
    for (auto &child : xNode.getChildren()) { stringifyNodes(child, destination, indent); }
  }
  // XML declaration
  else if (isA<Declaration>(xNode)) {
    auto &xNodeDeclaration = NRef<Declaration>(xNode);
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
  }
  // XML root or child elements
  else if (isA<Root>(xNode) || isA<Element>(xNode) || isA<Self>(xNode)) {
    const auto &xElement = NRef<Element>(xNode);
    destination.add("<" + xElement.name());
    for (auto &attribute : xElement.getAttributes()) {
      destination.add(" " + attribute.getName() + "=" + attribute.getQuote() + attribute.getUnparsed() + attribute.getQuote());
    }
    if (!isA<Self>(xNode)) {
      destination.add(">");
      for (auto &child : xNode.getChildren()) { stringifyNodes(child, destination, indent); }
      destination.add("</" + xElement.name() + ">");
    } else {
      destination.add("/>");
    }
  }
  // XML comments
  else if (isA<Comment>(xNode)) {
    const auto &xNodeComment = NRef<Comment>(xNode);
    destination.add("<!--" + xNodeComment.value() + "-->");
  }
  // XML element content
  else if (isA<Content>(xNode)) {
    const auto &xNodeContent = NRef<Content>(xNode);
    destination.add(xNodeContent.value());
  }
  // XML character entity
  else if (isA<EntityReference>(xNode)) {
    const auto &xNodeEntity = NRef<EntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
  }
  // XML processing instruction
  else if (isA<PI>(xNode)) {
    const PI &xNodePI = NRef<PI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
  }
  // XML CDATA section
  else if (isA<CDATA>(xNode)) {
    const auto &xNodeCDATA = NRef<CDATA>(xNode);
    destination.add("<![CDATA[" + std::string(xNodeCDATA.value()) + "]]>");
  }
  // XML DTD_Validator
  else if (isA<DTD>(xNode)) {
    destination.add(NRef<DTD>(xNode).unparsed());
  } else {
    throw Error("Invalid Node encountered during stringify.");
  }
}

};
}// namespace XML_Lib
