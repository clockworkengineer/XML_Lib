#pragma once

namespace XML_Lib {

class IEntityResolver;

/// @brief Segregated interface for external entity policy and XXE security enforcement.
class ISecurityPolicyManager
{
public:
  virtual ~ISecurityPolicyManager() noexcept = default;

  /// @brief Set external entity resolution policy and custom resolver.
  virtual void setExternalEntityPolicy(bool allowExternal, IEntityResolver *resolver) = 0;
};

} // namespace XML_Lib
