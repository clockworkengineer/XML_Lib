#pragma once

namespace XML_Lib {

struct XProlog : XNode
{
  // Constructors/Destructors
  XProlog() : XNode(XNode::Type::prolog) {}
  XProlog(const XProlog &other) = delete;
  XProlog &operator=(const XProlog &other) = delete;
  XProlog(XProlog &&other) = default;
  XProlog &operator=(XProlog &&other) = default;
  ~XProlog() = default;
};
}// namespace XML_Lib