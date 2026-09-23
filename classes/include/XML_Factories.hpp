#pragma once

#include "interface/IParser.hpp"
#include "interface/IStringify.hpp"
#include "interface/IEntityMapper.hpp"
#include "interface/IValidatorRegistry.hpp"
#if defined(XML_LIB_ENABLE_XPATH)
#include "interface/IXPathEngine.hpp"
#endif
#include <memory>

namespace XML_Lib {

/// @brief Factory creating the default entity mapper.
[[nodiscard]] std::unique_ptr<IEntityMapper> createDefaultEntityMapper();

/// @brief Factory creating the default validating XML parser.
[[nodiscard]] std::unique_ptr<IValidatingParser> createDefaultParser(IEntityMapper &entityMapper);

#if defined(XML_LIB_ENABLE_STRINGIFY)
/// @brief Factory creating the default XML stringifier.
[[nodiscard]] std::unique_ptr<IStringify> createDefaultStringify();
#endif

/// @brief Factory creating the default validator registry.
[[nodiscard]] std::unique_ptr<IValidatorRegistry> createDefaultValidatorRegistry();

#if defined(XML_LIB_ENABLE_XPATH)
/// @brief Factory creating the default XPath query engine.
[[nodiscard]] std::unique_ptr<IXPathEngine> createDefaultXPathEngine();
#endif

} // namespace XML_Lib
