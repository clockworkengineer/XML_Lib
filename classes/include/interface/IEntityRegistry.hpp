#pragma once

#include "XML_Types.hpp"
#include <string>
#include <string_view>
#include <stdexcept>

namespace XML_Lib {

struct XMLExternalReference;

/// @brief Segregated interface for XML entity reference registration and lookup.
class IEntityRegistry
{
public:
  virtual ~IEntityRegistry() noexcept = default;

  /// @brief Check if entity is internal.
  virtual bool isInternal(const std::string_view &entityName) = 0;

  /// @brief Check if entity is external.
  virtual bool isExternal(const std::string_view &entityName) = 0;

  /// @brief Check if entity is a notation.
  virtual bool isNotation(const std::string_view &entityName) = 0;

  /// @brief Get internal entity value.
  virtual const std::string &getInternal(const std::string_view &entityName) = 0;

  /// @brief Get notation entity value.
  virtual const std::string &getNotation(const std::string_view &entityName) = 0;

  /// @brief Get external entity reference details.
  virtual const XMLExternalReference &getExternal(const std::string_view &entityName) = 0;

  /// @brief Register an internal entity.
  virtual void setInternal(const std::string_view &entityName, const std::string_view &internal) = 0;

  /// @brief Register a notation.
  virtual void setNotation(const std::string_view &entityName, const std::string_view &notation) = 0;

  /// @brief Register an external entity.
  virtual void setExternal(const std::string_view &entityName, const XMLExternalReference &external) = 0;

  /// @brief Check if an entity is registered.
  [[nodiscard]] virtual bool isPresent(const std::string_view &entityName) const = 0;

  /// @brief Reset the entity registry to default state.
  virtual void reset() = 0;
};

} // namespace XML_Lib
