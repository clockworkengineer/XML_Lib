//
// Class: XML_Impl
//
// Description:  Code to take an XML internal object description and produce
// the XML text for it on a destination stream. Currently as UTF-8 encoded
// text but this may change in future.
//
// Dependencies:   C++20 - Language standard features used.
//

#include "XML_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Recursively parse XNode passed in to produce XML output on a
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="xNode">XNode to convert into XML.</param>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringifyElements(XNode &xNode, IDestination &destination)
{
  // XML prolog
  if (xNode.isProlog()) {
    for (auto &element : xNode.getChildren()) { stringifyElements(element, destination); }
  }
  // XML declaration
  else if (xNode.isDeclaration()) {
    XDeclaration &xNodeDeclaration = XRef<XDeclaration>(declaration());
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
  }
  // XML root or child elements
  else if (xNode.isRoot() || xNode.isElement()) {
    XElement &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto attr : xElement.getAttributeList()) {
      destination.add(" " + attr.getName() + "=\"" + attr.getUnparsed() + "\"");
    }
    destination.add(">");
    for (auto &element : xNode.getChildren()) { stringifyElements(element, destination); }
    destination.add("</" + xElement.name() + ">");
  }
  // Self closing element
  else if (xNode.isSelf()) {
    XElement &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto attr : xElement.getAttributeList()) {
      destination.add(" " + attr.getName() + "=\"" + attr.getUnparsed() + "\"");
    }
    destination.add("/>");
  }
  // XML comments
  else if (xNode.isComment()) {
    XComment &xNodeComment = XRef<XComment>(xNode);
    destination.add("<!--" + xNodeComment.comment() + "-->");
  }
  // XML element content
  else if (xNode.isContent()) {
    XContent &xNodeContent = XRef<XContent>(xNode);
    destination.add(xNodeContent.getContent());
  }
  // XML character entity
  else if (xNode.isEntity()) {
    XEntityReference &xNodeEntity = XRef<XEntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
  }
  // XML processing instruction
  else if (xNode.isPI()) {
    XPI &xNodePI = XRef<XPI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
  }
  // XML CDATA section
  else if (xNode.isCDATA()) {
    XCDATA &xNodeCDATA = XRef<XCDATA>(xNode);
    destination.add("<![CDATA[" + xNodeCDATA.CDATA() + "]]>");

  }
  // XML DTD
  else if (xNode.isDTD()) {
    destination.add(XRef<XDTD>(dtd()).unparsed());
  } else {
    throw XML::Error("Invalid XNode encountered during stringify.");
  }
}

/// <summary>
/// Recursively parse XNode passed in to produce XML output on a
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringifyXML(IDestination &destination) { stringifyElements(xmlRoot, destination); }
}// namespace XML_Lib
