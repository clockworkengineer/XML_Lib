//
// Class: XML_Impl
//
// Description:  Code to take an XML internal object description and produce
// the XML text for it on a destination stream. Currently as UTF-8 encoded 
// text but this may change in future.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML_Impl.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===========================
// PRIVATE TYPES AND CONSTANTS
// ===========================
// ==========================
// PUBLIC TYPES AND CONSTANTS
// ==========================
// ========================
// PRIVATE STATIC VARIABLES
// ========================
// =======================
// PUBLIC STATIC VARIABLES
// =======================
// ===============
// PRIVATE METHODS
// ===============
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
  case XNode::Type::prolog: {
    for (auto &element : xNode.getChildren()) { stringifyElements(*element, destination); }
    break;
  }
  // XML declaration
  case XNode::Type::declaration: {
    XDeclaration &xNodeDeclaration = XRef<XDeclaration>(declaration());
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
    break;
  }
  // XML root or child elements
  case XNode::Type::root:
  case XNode::Type::element: {
    XElement &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto attr : xElement.getAttributeList()) {
      destination.add(" " + attr.name + "=\"" + attr.value.getUnparsed() + "\"");
    }
    destination.add(">");
    for (auto &element : xNode.getChildren()) { stringifyElements(*element, destination); }
    destination.add("</" + xElement.name() + ">");
    break;
  }
  // Self closing element
  case XNode::Type::self: {
    XElement &xElement = XRef<XElement>(xNode);
    destination.add("<" + xElement.name());
    for (auto attr : xElement.getAttributeList()) {
      destination.add(" " + attr.name + "=\"" + attr.value.getUnparsed() + "\"");
    }
    destination.add("/>");
    break;
  }
  // XML comments
  case XNode::Type::comment: {
    XComment &xNodeComment = XRef<XComment>(xNode);
    destination.add("<!--" + xNodeComment.comment() + "-->");
    break;
  }
  // XML element content
  case XNode::Type::content: {
    XContent &xNodeContent = XRef<XContent>(xNode);
    destination.add(xNodeContent.getContent());
    break;
  }
  // XML character entity
  case XNode::Type::entity: {
    XEntityReference &xNodeEntity = XRef<XEntityReference>(xNode);
    destination.add(xNodeEntity.value().getUnparsed());
    break;
  }
  // XML processing instruction
  case XNode::Type::pi: {
    XPI &xNodePI = XRef<XPI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
    break;
  }
  // XML CDATA section
  case XNode::Type::cdata: {
    XCDATA &xNodeCDATA = XRef<XCDATA>(xNode);
    destination.add("<![CDATA[" + xNodeCDATA.CDATA() + "]]>");
    break;
  }
  // XML DTD
  case XNode::Type::dtd: {
    destination.add(XRef<XDTD>(dtd()).unparsed());
    break;
  }
  default:
    throw XML_Lib::Error("Invalid XNode encountered during stringify.");
  }
}
/// <summary>
/// Recursively parse XNode passed in to produce XML output on a
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringifyXML(IDestination &destination) { stringifyElements(*m_prolog, destination); }
}// namespace XML_Lib
