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
  switch (xNode.getType()) {
  // XML prolog
  case Variant::Type::prolog: {
    for (auto &element : xNode.getChildren()) { stringifyElements(element, destination); }
    break;
  }
  // XML declaration
  case Variant::Type::declaration: {
    XDeclaration &xNodeDeclaration = XRef<XDeclaration>(declaration());
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
    break;
  }
  // XML root or child elements
  case Variant::Type::root:
  case Variant::Type::element: {
    XElement &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto attr : xElement.getAttributeList()) {
      destination.add(" " + attr.getName() + "=\"" + attr.getUnparsed() + "\"");
    }
    destination.add(">");
    for (auto &element : xNode.getChildren()) { stringifyElements(element, destination); }
    destination.add("</" + xElement.name() + ">");
    break;
  }
  // Self closing element
  case Variant::Type::self: {
    XElement &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto attr : xElement.getAttributeList()) {
      destination.add(" " + attr.getName() + "=\"" + attr.getUnparsed() + "\"");
    }
    destination.add("/>");
    break;
  }
  // XML comments
  case Variant::Type::comment: {
    XComment &xNodeComment = XRef<XComment>(xNode);
    destination.add("<!--" + xNodeComment.comment() + "-->");
    break;
  }
  // XML element content
  case Variant::Type::content: {
    XContent &xNodeContent = XRef<XContent>(xNode);
    destination.add(xNodeContent.getContent());
    break;
  }
  // XML character entity
  case Variant::Type::entity: {
    XEntityReference &xNodeEntity = XRef<XEntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
    break;
  }
  // XML processing instruction
  case Variant::Type::pi: {
    XPI &xNodePI = XRef<XPI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
    break;
  }
  // XML CDATA section
  case Variant::Type::cdata: {
    XCDATA &xNodeCDATA = XRef<XCDATA>(xNode);
    destination.add("<![CDATA[" + xNodeCDATA.CDATA() + "]]>");
    break;
  }
  // XML DTD
  case Variant::Type::dtd: {
    destination.add(XRef<XDTD>(dtd()).unparsed());
    break;
  }
  default:
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
