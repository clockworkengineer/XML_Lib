//
// Class: XML_Impl
//
// Description: Miscellaneous XML class implementation code.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Parse entity reference as XML and add XNodes produced to the current XNode.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <param name="entityReference">Entity reference to be parsed for XML.</param>
void XML_Impl::processEntityReferenceXML(XNode &xNode, const XMLValue &entityReference)
{
  auto xElement = XNode::make<XElement>();
  BufferSource entitySource(entityReference.getParsed());
  // Parse entity XML
  while (entitySource.more()) { parseElementInternal(entitySource, xNode); }
  // Place into XNode (element) child list
  for (auto &xNodeChild : xElement.getChildren()) { xNode.addChild(xNodeChild); }
}

/// <summary>
/// Reset content node whitespace flag if set.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void XML_Impl::resetWhiteSpace(XNode &xNode)
{
  if (!xNode.getChildren().empty()) {
    if (xNode.getChildren().back().isContent()) { XRef<XContent>(xNode.getChildren().back()).setIsWhiteSpace(false); }
  }
}

/// <summary>
/// Add content XNode to elements child list.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <param name="content">Content to add to new content XNode (XMLNodeContent).</param>
void XML_Impl::addContentToElementChildList(XNode &xNode, const std::string &content)
{
  // Make sure there is a content XNode to receive characters
  if (xNode.getChildren().empty() || !xNode.getChildren().back().isContent()) {
    bool isWhiteSpace = true;
    if (!xNode.getChildren().empty()) {
      if ((xNode.getChildren().back().isCDATA()) || (xNode.getChildren().back().isEntity())) { isWhiteSpace = false; }
    }
    xNode.addChild(XNode::make<XContent>(isWhiteSpace));
  }
  XContent &xmlContent = XRef<XContent>(xNode.getChildren().back());
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

/// <summary>
/// Add new namespaces to current global list.
/// </summary>
/// <param name="attributes">Current element attrutes..</param>
/// <param name="namespaces">Current global namespace list.</param>
void XML_Impl::addNewNameSpaces(const std::vector<XMLAttribute> &attributes, std::vector<XMLAttribute> &namespaces)
{
  for (const auto &attribute : attributes) {
    if (attribute.getName().starts_with("xmlns")) {
      namespaces.emplace_back((attribute.getName().size() > 5) ? attribute.getName().substr(6) : ":",
        XMLValue{ attribute.getUnparsed(), attribute.getValue() });
    }
  }
}
}// namespace XML_Lib
