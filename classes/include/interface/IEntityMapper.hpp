#pragma once

#include "IEntityRegistry.hpp"
#include "IEntityExpander.hpp"
#include "ISecurityPolicyManager.hpp"
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief Composite interface for XML entity mapping, translation, cycle checking, and security enforcement.
class IEntityMapper : public IEntityRegistry, public IEntityExpander, public ISecurityPolicyManager
{
public:
  /// @brief Exception thrown when an entity mapping error occurs.
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IEntityMapper Error: ").append(message)) {}
  };

  ~IEntityMapper() noexcept override = default;
};

} // namespace XML_Lib