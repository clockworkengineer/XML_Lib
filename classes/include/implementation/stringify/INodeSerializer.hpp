#pragma once

#include <functional>
#include <memory>
#include <typeindex>

namespace XML_Lib {

struct Node;
class IDestination;

/// @brief Strategy interface for serializing a specific XML node type (OCP).
class INodeSerializer
{
public:
  virtual ~INodeSerializer() noexcept = default;

  /// @brief Serialize a specific node into destination stream.
  virtual void serialize(
    const Node &xNode,
    IDestination &destination,
    unsigned long indent,
    const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const = 0;
};

} // namespace XML_Lib
