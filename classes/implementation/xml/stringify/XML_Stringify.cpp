//
// Class: XML_Stringify
//
// Description:  Code to take an XML internal object description and produce
// the XML text for it on a destination stream. Currently as UTF-8 encoded
// text but this may change in future.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Stringify.hpp"

namespace XML_Lib {

/// <summary>
/// Recursively parse XNode passed in to produce XML output on a
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="xNode">XNode to convert into XML.</param>
/// <param name="destination">XML destination stream.</param>
void XML_Stringify::stringify(const XNode &xNode, IDestination &destination) const
{
  // XML prolog
  if (xNode.isProlog()) {
    for (auto &child : xNode.getChildren()) { stringify(child, destination); }
  }
  // XML declaration
  else if (xNode.isDeclaration()) {
    auto &xNodeDeclaration = XRef<XDeclaration>(xNode);
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
  }
  // XML root or child elements
  else if (xNode.isRoot() || xNode.isElement() || xNode.isSelf()) {
    const XElement &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto &attribute : xElement.getAttributeList()) {
      destination.add(" " + attribute.getName() + "=\"" + attribute.getUnparsed() + "\"");
    }
    if (!xNode.isSelf()) {
      destination.add(">");
      for (auto &child : xNode.getChildren()) { stringify(child, destination); }
      destination.add("</" + xElement.name() + ">");
    } else {
      destination.add("/>");
    }
  }
  // XML comments
  else if (xNode.isComment()) {
    const XComment &xNodeComment = XRef<XComment>(xNode);
    destination.add("<!--" + xNodeComment.comment() + "-->");
  }
  // XML element content
  else if (xNode.isContent()) {
    const XContent &xNodeContent = XRef<XContent>(xNode);
    destination.add(xNodeContent.getContent());
  }
  // XML character entity
  else if (xNode.isEntity()) {
    const XEntityReference &xNodeEntity = XRef<XEntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
  }
  // XML processing instruction
  else if (xNode.isPI()) {
    const XPI &xNodePI = XRef<XPI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
  }
  // XML CDATA section
  else if (xNode.isCDATA()) {
    const XCDATA &xNodeCDATA = XRef<XCDATA>(xNode);
    destination.add("<![CDATA[" + xNodeCDATA.CDATA() + "]]>");
  }
  // XML DTD
  else if (xNode.isDTD()) {
    destination.add(XRef<XDTD>(xNode).unparsed());
  } else {
    throw Error("Invalid XNode encountered during stringify.");
  }
}
}// namespace XML_Lib