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
// =========
// NAMESPACE
// =========
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
/// <param name="xmlNode">Current element node.</param>
/// <param name="entityReference">Entity reference to be parsed fro XML.</param>
void XML_Impl::processEntityReferenceXML(XMLNode &xmlNode, const XMLValue &entityReference)
{
  XMLNodeElement entityElement;
  BufferSource entitySource(entityReference.parsed);
  // Parse entity XML
  while (entitySource.more()) { parseElementContents(entitySource, entityElement); }
  // Place into node (element) child list
  for (auto &xmlNodePtr : entityElement.children) { xmlNode.children.emplace_back(std::move(xmlNodePtr)); }
}
/// <summary>
/// </summary>
/// <param name="xmlNode">Current element node.</param>
void XML_Impl::resetWhiteSpace(XMLNode &xmlNode)
{
  if (!xmlNode.children.empty()) {
    if (xmlNode.children.back()->getNodeType() == XMLNodeType::content) {
      XMLNodeRef<XMLNodeContent>(*xmlNode.children.back()).setIsWhiteSpace(false);
    }
  }
}
/// <summary>
/// Add content node to elements child list.
/// </summary>
/// <param name="xmlNode">Current element node.</param>
/// <param name="content">Content to add to new content node (XMLNodeContent).</param>
void XML_Impl::addContentToElementChildList(XMLNode &xmlNode, const std::string &content)
{
  // Make sure there is a content node to receive characters
  if (xmlNode.children.empty() || xmlNode.children.back()->getNodeType() != XMLNodeType::content) {
    bool isWhiteSpace = true;
    if (!xmlNode.children.empty()) {
      if ((xmlNode.children.back()->getNodeType() == XMLNodeType::cdata)
          || (xmlNode.children.back()->getNodeType() == XMLNodeType::entity)) {
        isWhiteSpace = false;
      }
    }
    xmlNode.children.emplace_back(std::make_unique<XMLNodeContent>(isWhiteSpace));
  }
  XMLNodeContent &xmlContent = XMLNodeRef<XMLNodeContent>(*xmlNode.children.back());
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
