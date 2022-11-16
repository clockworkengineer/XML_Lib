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
  XNodeElement entityElement;
  BufferSource entitySource(entityReference.parsed);
  // Parse entity XML
  while (entitySource.more()) { parseElementContents(entitySource, entityElement); }
  // Place into node (element) child list
  for (auto &xNodePtr : entityElement.children) { xNode.children.emplace_back(std::move(xNodePtr)); }
}
/// <summary>
/// </summary>
/// <param name="xNode">Current element node.</param>
void XML_Impl::resetWhiteSpace(XNode &xNode)
{
  if (!xNode.children.empty()) {
    if (xNode.children.back()->getNodeType() == XNodeType::content) {
      XNodeRef<XNodeContent>(*xNode.children.back()).setIsWhiteSpace(false);
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
  if (xNode.children.empty() || xNode.children.back()->getNodeType() != XNodeType::content) {
    bool isWhiteSpace = true;
    if (!xNode.children.empty()) {
      if ((xNode.children.back()->getNodeType() == XNodeType::cdata)
          || (xNode.children.back()->getNodeType() == XNodeType::entity)) {
        isWhiteSpace = false;
      }
    }
    xNode.children.emplace_back(std::make_unique<XNodeContent>(isWhiteSpace));
  }
  XNodeContent &xmlContent = XNodeRef<XNodeContent>(*xNode.children.back());
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
