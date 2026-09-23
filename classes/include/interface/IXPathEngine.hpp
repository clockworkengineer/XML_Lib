#pragma once

#include <string_view>
#include <vector>

namespace XML_Lib {

struct Node;

/// @brief Interface for XML query/path navigation engines (e.g. XPath 1.0, 2.0, CSS selectors).
///
/// Follows the Open/Closed Principle (OCP) by decoupling document querying from a specific engine.
class IXPathEngine
{
public:
  virtual ~IXPathEngine() noexcept = default;

  /// @brief Evaluate expression against contextNode and return pointers to matching nodes.
  [[nodiscard]] virtual std::vector<const Node *> evaluate(const Node &contextNode, std::string_view expression) const = 0;
};

} // namespace XML_Lib
