#pragma once

#include <string>
#include <vector>

namespace XML_Lib {

struct Node;

/// @brief Strategy role interface for abstracting DOM node navigation in XPath evaluation (DIP & OCP).
class IXPathNodeAdapter
{
public:
  virtual ~IXPathNodeAdapter() noexcept = default;

  /// @brief Get node name.
  [[nodiscard]] virtual std::string getName(const Node &node) const = 0;

  /// @brief Get node text value.
  [[nodiscard]] virtual std::string getValue(const Node &node) const = 0;

  /// @brief Get child nodes.
  [[nodiscard]] virtual std::vector<const Node *> getChildren(const Node &node) const = 0;
};

} // namespace XML_Lib
