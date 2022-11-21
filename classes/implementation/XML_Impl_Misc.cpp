//
// Class: XML_Impl
//
// Description: Miscellaneous XML class implementation code.
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
/// </summary>
/// <param name="xNode">Current element node.</param>
/// <param name="entityReference">Entity reference to be parsed fro XML.</param>
void XML_Impl::processEntityReferenceXML(XNode &xNode, const XMLValue &entityReference)
{
  XElement entityElement;
  BufferSource entitySource(entityReference.parsed);
  // Parse entity XML
  while (entitySource.more()) { parseElementContents(entitySource, entityElement); }
  // Place into node (element) child list
  for (auto &xNodePtr : entityElement.getChildren()) { xNode.getChildren().emplace_back(std::move(xNodePtr)); }
}
/// <summary>
/// </summary>
/// <param name="xNode">Current element node.</param>
void XML_Impl::resetWhiteSpace(XNode &xNode)
{
  if (!xNode.getChildren().empty()) {
    if (xNode.getChildren().back()->getType() == XNode::Type::content) {
      XRef<XContent>(*xNode.getChildren().back()).setIsWhiteSpace(false);
    }
  }
}
/// <summary>
/// Add content node to elements child list.
/// </summary>
/// <param name="xNode">Current element node.</param>
/// <param name="content">Content to add to new content node (XMLNodeContent).</param>
void XML_Impl::addContentToElementChildList(XNode &xNode, const std::string &content)
{
  // Make sure there is a content node to receive characters
  if (xNode.getChildren().empty() || xNode.getChildren().back()->getType() != XNode::Type::content) {
    bool isWhiteSpace = true;
    if (!xNode.getChildren().empty()) {
      if ((xNode.getChildren().back()->getType() == XNode::Type::cdata)
          || (xNode.getChildren().back()->getType() == XNode::Type::entity)) {
        isWhiteSpace = false;
      }
    }
    xNode.getChildren().emplace_back(std::make_unique<XContent>(isWhiteSpace));
  }
  XContent &xmlContent = XRef<XContent>(*xNode.getChildren().back());
  if (xmlContent.isWhiteSpace()) {
    for (const auto ch : content) {
      if (!std::iswspace(ch)) {
        xmlContent.setIsWhiteSpace(false);
        break;
      }
    }
  }
  xmlContent.addContent(content);
}
}// namespace XML_Lib
