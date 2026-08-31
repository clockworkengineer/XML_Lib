#pragma once

namespace XML_Lib {

class ISource;

/// @brief Segregated role interface for schema/grammar parsers (DTD, XSD, RNG, etc.).
template <typename SchemaType>
class ISchemaParser
{
public:
  virtual ~ISchemaParser() noexcept = default;

  /// @brief Parse schema grammar from source input into schema model representation.
  virtual SchemaType parseSchema(ISource &source) = 0;
};

} // namespace XML_Lib
