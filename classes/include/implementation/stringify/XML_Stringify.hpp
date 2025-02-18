#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

class XML_Stringify final : public IStringify
{
public:
  // Constructors/Destructors
  XML_Stringify() = default;
  XML_Stringify(const XML_Stringify &other) = delete;
  XML_Stringify &operator=(const XML_Stringify &other) = delete;
  XML_Stringify(XML_Stringify &&other) = delete;
  XML_Stringify &operator=(XML_Stringify &&other) = delete;
  ~XML_Stringify() override = default;

/// <summary>
/// Recursively parse XNode passed in to produce XML output on a
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="xNode">XNode to convert into XML.</param>
/// <param name="destination">XML destination stream.</param>
void stringify(const XNode &xNode, IDestination &destination, unsigned long indent) const override
  {
  // XML prolog
  if (isA<Prolog>(xNode)) {
    for (auto &child : xNode.getChildren()) { stringify(child, destination, indent); }
  }
  // XML declaration
  else if (isA<Declaration>(xNode)) {
    auto &xNodeDeclaration = XRef<Declaration>(xNode);
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
  }
  // XML root or child elements
  else if (isA<Root>(xNode) || isA<Element>(xNode) || isA<Self>(xNode)) {
    const auto &xElement = XRef<Element>(xNode);
    destination.add("<" + xElement.name());
    for (auto &attribute : xElement.getAttributes()) {
      destination.add(" " + attribute.getName() + "=" + attribute.getQuote() + attribute.getUnparsed() + attribute.getQuote());
    }
    if (!isA<Self>(xNode)) {
      destination.add(">");
      for (auto &child : xNode.getChildren()) { stringify(child, destination, indent); }
      destination.add("</" + xElement.name() + ">");
    } else {
      destination.add("/>");
    }
  }
  // XML comments
  else if (isA<Comment>(xNode)) {
    const auto &xNodeComment = XRef<Comment>(xNode);
    destination.add("<!--" + xNodeComment.value() + "-->");
  }
  // XML element content
  else if (isA<Content>(xNode)) {
    const auto &xNodeContent = XRef<Content>(xNode);
    destination.add(xNodeContent.value());
  }
  // XML character entity
  else if (isA<EntityReference>(xNode)) {
    const auto &xNodeEntity = XRef<EntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
  }
  // XML processing instruction
  else if (isA<PI>(xNode)) {
    const PI &xNodePI = XRef<PI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
  }
  // XML CDATA section
  else if (isA<CDATA>(xNode)) {
    const auto &xNodeCDATA = XRef<CDATA>(xNode);
    destination.add("<![CDATA[" + xNodeCDATA.value() + "]]>");
  }
  // XML DTD_Validator
  else if (isA<DTD>(xNode)) {
    destination.add(XRef<DTD>(xNode).unparsed());
  } else {
    throw Error("Invalid XNode encountered during stringify.");
  }
}

};
}// namespace XML_Lib
