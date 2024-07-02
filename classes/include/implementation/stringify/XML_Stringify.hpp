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
void stringify(const XNode &xNode, IDestination &destination) const override
  {
  // XML prolog
  if (isA<XProlog>(xNode)) {
    for (auto &child : xNode.getChildren()) { stringify(child, destination); }
  }
  // XML declaration
  else if (isA<XDeclaration>(xNode)) {
    auto &xNodeDeclaration = XRef<XDeclaration>(xNode);
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
  }
  // XML root or child elements
  else if (isA<XRoot>(xNode) || isA<XElement>(xNode) || isA<XSelf>(xNode)) {
    const auto &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto &attribute : xElement.getAttributes()) {
      destination.add(" " + attribute.getName() + "=" + attribute.getQuote() + attribute.getUnparsed() + attribute.getQuote());
    }
    if (!isA<XSelf>(xNode)) {
      destination.add(">");
      for (auto &child : xNode.getChildren()) { stringify(child, destination); }
      destination.add("</" + xElement.name() + ">");
    } else {
      destination.add("/>");
    }
  }
  // XML comments
  else if (isA<XComment>(xNode)) {
    const auto &xNodeComment = XRef<XComment>(xNode);
    destination.add("<!--" + xNodeComment.value() + "-->");
  }
  // XML element content
  else if (isA<XContent>(xNode)) {
    const auto &xNodeContent = XRef<XContent>(xNode);
    destination.add(xNodeContent.value());
  }
  // XML character entity
  else if (isA<XEntityReference>(xNode)) {
    const auto &xNodeEntity = XRef<XEntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
  }
  // XML processing instruction
  else if (isA<XPI>(xNode)) {
    const XPI &xNodePI = XRef<XPI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
  }
  // XML CDATA section
  else if (isA<XCDATA>(xNode)) {
    const auto &xNodeCDATA = XRef<XCDATA>(xNode);
    destination.add("<![CDATA[" + xNodeCDATA.value() + "]]>");
  }
  // XML DTD
  else if (isA<XDTD>(xNode)) {
    destination.add(XRef<XDTD>(xNode).unparsed());
  } else {
    throw Error("Invalid XNode encountered during stringify.");
  }
}

};
}// namespace XML_Lib
