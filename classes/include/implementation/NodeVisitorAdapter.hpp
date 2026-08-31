#pragma once

#include "interface/IVisitorRoles.hpp"
#include "XML_NodeRef.hpp"

namespace XML_Lib {

/// @brief Adapter delegating tree node visits to narrow role visitors (ISP compliant).
class NodeVisitorAdapter
{
public:
  template <typename VisitorT, typename NodeT>
  static void dispatchVisit(NodeT &xNode, VisitorT &visitor)
  {
    if constexpr (std::is_base_of_v<INodeVisitor, VisitorT>) {
      visitor.onNode(xNode);
    }

    if (isA<Prolog>(xNode)) {
      if constexpr (std::is_base_of_v<IPrologVisitor, VisitorT>) { visitor.onProlog(xNode); }
    } else if (isA<Declaration>(xNode)) {
      if constexpr (std::is_base_of_v<IDeclarationVisitor, VisitorT>) { visitor.onDeclaration(xNode); }
    } else if (isA<Root>(xNode)) {
      if constexpr (std::is_base_of_v<IRootVisitor, VisitorT>) { visitor.onRoot(xNode); }
    } else if (isA<Self>(xNode)) {
      if constexpr (std::is_base_of_v<ISelfVisitor, VisitorT>) { visitor.onSelf(xNode); }
    } else if (isA<Element>(xNode)) {
      if constexpr (std::is_base_of_v<IElementVisitor, VisitorT>) { visitor.onElement(xNode); }
    } else if (isA<Content>(xNode)) {
      if constexpr (std::is_base_of_v<IContentVisitor, VisitorT>) { visitor.onContent(xNode); }
    } else if (isA<EntityReference>(xNode)) {
      if constexpr (std::is_base_of_v<IEntityReferenceVisitor, VisitorT>) { visitor.onEntityReference(xNode); }
    } else if (isA<Comment>(xNode)) {
      if constexpr (std::is_base_of_v<ICommentVisitor, VisitorT>) { visitor.onComment(xNode); }
    } else if (isA<CDATA>(xNode)) {
      if constexpr (std::is_base_of_v<ICDATAVisitor, VisitorT>) { visitor.onCDATA(xNode); }
    } else if (isA<PI>(xNode)) {
      if constexpr (std::is_base_of_v<IPIVisitor, VisitorT>) { visitor.onPI(xNode); }
    } else if (isA<DTD>(xNode)) {
      if constexpr (std::is_base_of_v<IDTDVisitor, VisitorT>) { visitor.onDTD(xNode); }
    }
  }
};

} // namespace XML_Lib
