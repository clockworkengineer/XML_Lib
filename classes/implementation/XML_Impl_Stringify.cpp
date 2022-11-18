//
// Class: XML_Impl
//
// Description:  Code to take an XML internal object description and produce
// the XML text for it; currently as UTF-8 encoded text but this may change in
// future.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML.hpp"
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
/// Recursively parse XMLNode passed in to produce XML output on an XML
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="xNode">XMLNode to convert into XML.</param>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringifyElements(XNode &xNode, IDestination &destination)
{
  switch (xNode.getNodeType()) {
  // XML prolog
  case XNode::Type::prolog: {
    for (auto &element : xNode.children) { stringifyElements(*element, destination); }
    break;
  }
  // XML declaration
  case XNode::Type::declaration: {
    XNodeDeclaration &xNodeDeclaration = XNodeRef<XNodeDeclaration>(declaration());
    destination.add("<?xml version=\"" + xNodeDeclaration.version() + "\"" + " encoding=\""
                    + xNodeDeclaration.encoding() + "\"" + " standalone=\"" + xNodeDeclaration.standalone() + "\"?>");
    break;
  }
  // XML root or child elements
  case XNode::Type::root:
  case XNode::Type::element: {
    XNodeElement &xNodeElement = XNodeRef<XNodeElement>(xNode);
    destination.add("<" + xNodeElement.name());
    for (auto attr : xNodeElement.getAttributeList()) {
      destination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
    }
    destination.add(">");
    for (auto &element : xNode.children) { stringifyElements(*element, destination); }
    destination.add("</" + xNodeElement.name() + ">");
    break;
  }
  // Self closing element
  case XNode::Type::self: {
    XNodeElement &xNodeElement = XNodeRef<XNodeElement>(xNode);
    destination.add("<" + xNodeElement.name());
    for (auto attr : xNodeElement.getAttributeList()) {
      destination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
    }
    destination.add("/>");
    break;
  }
  // XML comments
  case XNode::Type::comment: {
    XNodeComment &xNodeComment = XNodeRef<XNodeComment>(xNode);
    destination.add("<!--" + xNodeComment.comment() + "-->");
    break;
  }
  // XML element content
  case XNode::Type::content: {
    XNodeContent &xNodeContent = XNodeRef<XNodeContent>(xNode);
    destination.add(xNodeContent.content());
    break;
  }
  // XML character entity
  case XNode::Type::entity: {
    XNodeEntityReference &xNodeEntity = XNodeRef<XNodeEntityReference>(xNode);
    destination.add(xNodeEntity.value().unparsed);
    break;
  }
  // XML processing instruction
  case XNode::Type::pi: {
    XNodePI &xNodePI = XNodeRef<XNodePI>(xNode);
    destination.add("<?" + xNodePI.name() + " " + xNodePI.parameters() + "?>");
    break;
  }
  // XML CDATA section
  case XNode::Type::cdata: {
    XNodeCDATA &xNodeCDATA = XNodeRef<XNodeCDATA>(xNode);
    destination.add("<![CDATA[" + xNodeCDATA.CDATA() + "]]>");
    break;
  }
  // XML DTD
  case XNode::Type::dtd: {
    destination.add(XNodeRef<XNodeDTD>(dtd()).unparsed());
    break;
  }
  default:
    throw XML_Lib::Error("Invalid XMLNode encountered during stringify.");
  }
}
/// <summary>
/// Recursively parse XMLNode passed in to produce XML output on an XML
/// destination stream in UTF-8 encoding.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringifyXML(IDestination &destination) { stringifyElements(*m_prolog, destination); }
}// namespace XML_Lib
