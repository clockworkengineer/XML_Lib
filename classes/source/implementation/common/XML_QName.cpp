#include "common/XML_QName.hpp"
#include "XML_Error.hpp"

namespace XML_Lib {

bool isValidQName(std::string_view qname) noexcept
{
  const auto pos = qname.find(':');
  if (pos == std::string_view::npos) {
    return !qname.empty();
  }
  if (pos == 0 || pos + 1 >= qname.size() || qname.find(':', pos + 1) != std::string_view::npos) {
    return false;
  }
  return true;
}

void validateQName(std::string_view qname, std::string_view contextName, const ISource &source)
{
  if (const auto pos = qname.find(':'); pos != std::string_view::npos) {
    if (pos == 0 || pos + 1 >= qname.size() || qname.find(':', pos + 1) != std::string_view::npos) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid QName in " + std::string(contextName) + ": '" + std::string(qname) + "'."));
    }
  }
}

} // namespace XML_Lib
