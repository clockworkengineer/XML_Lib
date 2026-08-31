#pragma once

namespace XML_Lib {

struct Node;

/// @brief Segregated role interface for schema document validation execution.
class ISchemaValidator
{
public:
  virtual ~ISchemaValidator() noexcept = default;

  /// @brief Validate target DOM node tree against active schema rules.
  virtual void validateDocument(const Node &rootNode) = 0;
};

} // namespace XML_Lib
