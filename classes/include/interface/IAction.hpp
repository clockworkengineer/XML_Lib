#pragma once

#include "IVisitorRoles.hpp"
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief Visitor interface for tree traversal events.
///
/// Derive from `IAction` and override the `on*` methods you care about, then pass
/// an instance to `XML::traverse()`.  Each node type generates a matching callback
/// with both mutable and const overloads.
class IAction : public INodeVisitor,
                public IElementVisitor,
                public ICommentVisitor,
                public IContentVisitor,
                public ICDATAVisitor,
                public IDeclarationVisitor,
                public IDTDVisitor,
                public IEntityReferenceVisitor,
                public IPIVisitor,
                public IPrologVisitor,
                public IRootVisitor,
                public ISelfVisitor
{
public:
  /// @brief Exception thrown when an action encounters an error during traversal.
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IAction Error: ").append(message)) {}
  };

  ~IAction() noexcept override = default;
};

} // namespace XML_Lib