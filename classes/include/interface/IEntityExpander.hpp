#pragma once

#include <set>
#include <string>
#include <string_view>

namespace XML_Lib {

struct XMLValue;

/// @brief Segregated interface for entity translation and macro substitution.
class IEntityExpander
{
public:
  virtual ~IEntityExpander() noexcept = default;

  /// @brief Map an entity reference value.
  virtual XMLValue map(const XMLValue &entityReference) = 0;

  /// @brief Translate entity references embedded within string @p toTranslate.
  [[nodiscard]] virtual std::string translate(const std::string_view &toTranslate, char type = '%') const = 0;

  /// @brief Check entity definition for recursive expansion cycles.
  virtual void checkRecursiveEntity(const std::string_view &entityName, const std::string &expanded, std::set<std::string> &currentEntities) = 0;

  /// @brief Check all registered entity mappings for recursion.
  virtual void checkForRecursion() = 0;
};

} // namespace XML_Lib
