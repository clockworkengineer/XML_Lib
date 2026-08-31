#pragma once

namespace XML_Lib {

struct Node;

class IElementVisitor {
public:
  virtual ~IElementVisitor() noexcept = default;
  virtual void onElement([[maybe_unused]] Node &node) {}
  virtual void onElement([[maybe_unused]] const Node &node) {}
};

class ICommentVisitor {
public:
  virtual ~ICommentVisitor() noexcept = default;
  virtual void onComment([[maybe_unused]] Node &node) {}
  virtual void onComment([[maybe_unused]] const Node &node) {}
};

class IContentVisitor {
public:
  virtual ~IContentVisitor() noexcept = default;
  virtual void onContent([[maybe_unused]] Node &node) {}
  virtual void onContent([[maybe_unused]] const Node &node) {}
};

class ICDATAVisitor {
public:
  virtual ~ICDATAVisitor() noexcept = default;
  virtual void onCDATA([[maybe_unused]] Node &node) {}
  virtual void onCDATA([[maybe_unused]] const Node &node) {}
};

class IDeclarationVisitor {
public:
  virtual ~IDeclarationVisitor() noexcept = default;
  virtual void onDeclaration([[maybe_unused]] Node &node) {}
  virtual void onDeclaration([[maybe_unused]] const Node &node) {}
};

class IDTDVisitor {
public:
  virtual ~IDTDVisitor() noexcept = default;
  virtual void onDTD([[maybe_unused]] Node &node) {}
  virtual void onDTD([[maybe_unused]] const Node &node) {}
};

class IEntityReferenceVisitor {
public:
  virtual ~IEntityReferenceVisitor() noexcept = default;
  virtual void onEntityReference([[maybe_unused]] Node &node) {}
  virtual void onEntityReference([[maybe_unused]] const Node &node) {}
};

class IPIVisitor {
public:
  virtual ~IPIVisitor() noexcept = default;
  virtual void onPI([[maybe_unused]] Node &node) {}
  virtual void onPI([[maybe_unused]] const Node &node) {}
};

class IPrologVisitor {
public:
  virtual ~IPrologVisitor() noexcept = default;
  virtual void onProlog([[maybe_unused]] Node &node) {}
  virtual void onProlog([[maybe_unused]] const Node &node) {}
};

class IRootVisitor {
public:
  virtual ~IRootVisitor() noexcept = default;
  virtual void onRoot([[maybe_unused]] Node &node) {}
  virtual void onRoot([[maybe_unused]] const Node &node) {}
};

class ISelfVisitor {
public:
  virtual ~ISelfVisitor() noexcept = default;
  virtual void onSelf([[maybe_unused]] Node &node) {}
  virtual void onSelf([[maybe_unused]] const Node &node) {}
};

class INodeVisitor {
public:
  virtual ~INodeVisitor() noexcept = default;
  virtual void onNode([[maybe_unused]] Node &node) {}
  virtual void onNode([[maybe_unused]] const Node &node) {}
};

} // namespace XML_Lib
