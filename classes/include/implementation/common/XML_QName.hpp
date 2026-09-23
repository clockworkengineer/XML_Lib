#pragma once

#include "XML_Interfaces.hpp"
#include <string>
#include <string_view>

namespace XML_Lib {

struct QNameView {
  std::string_view prefix;
  std::string_view localName;
  [[nodiscard]] constexpr bool hasPrefix() const noexcept { return !prefix.empty(); }
};

[[nodiscard]] constexpr QNameView splitQName(std::string_view qname) noexcept
{
  const auto pos = qname.find(':');
  if (pos == std::string_view::npos) {
    return { {}, qname };
  }
  return { qname.substr(0, pos), qname.substr(pos + 1) };
}

[[nodiscard]] constexpr std::string_view getLocalName(std::string_view qname) noexcept
{
  const auto pos = qname.find(':');
  return (pos == std::string_view::npos) ? qname : qname.substr(pos + 1);
}

[[nodiscard]] constexpr std::string_view getPrefix(std::string_view qname) noexcept
{
  const auto pos = qname.find(':');
  return (pos == std::string_view::npos) ? std::string_view{} : qname.substr(0, pos);
}

[[nodiscard]] bool isValidQName(std::string_view qname) noexcept;
void validateQName(std::string_view qname, std::string_view contextName, const ISource &source);

} // namespace XML_Lib
