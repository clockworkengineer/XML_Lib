#include "XML_EntityMapperHelpers.hpp"

namespace XML_Lib {

std::optional<std::pair<std::string_view, const XML_EntityMapping *>> matchEntityPrefix(
  const std::vector<std::pair<std::string_view, const XML_EntityMapping *>> &candidates,
  const std::string_view &text,
  const size_t pos)
{
  if (pos >= text.size()) { return std::nullopt; }
  for (const auto &[key, mapping] : candidates) {
    if (text.size() - pos >= key.size() && text.compare(pos, key.size(), key) == 0) {
      return std::pair<std::string_view, const XML_EntityMapping *>(key, mapping);
    }
  }
  return std::nullopt;
}

} // namespace XML_Lib
