#pragma once
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ======
// Prolog
// ======
struct XProlog : XNode
{
  // Constructors/Destructors
  explicit XProlog(XNode::Type nodeType = XNode::Type::prolog) : XNode(nodeType) {}
  XProlog(const XProlog &other) = delete;
  XProlog &operator=(XProlog &other) = delete;
  XProlog(XProlog &&other) = default;
  XProlog &operator=(XProlog &&other) = default;
  ~XProlog() = default;
};
}// namespace XML_Lib