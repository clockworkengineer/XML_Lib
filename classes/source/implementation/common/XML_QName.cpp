#include "common/XML_QName.hpp"
#include "XML_Error.hpp"

namespace XML_Lib {

bool isValidQName(std::string_view qname) noexcept
{
  if (!qname.contains(':')) {
    return !qname.empty();
  }
  const auto pos = qname.find(':');
  if (pos == 0 || pos + 1 >= qname.size() || qname.substr(pos + 1).contains(':')) {
    return false;
  }
  return true;
}

void validateQName(std::string_view qname, std::string_view contextName, const ISource &source)
{
  if (qname.contains(':')) {
    const auto pos = qname.find(':');
    if (pos == 0 || pos + 1 >= qname.size() || qname.substr(pos + 1).contains(':')) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid QName in " + std::string(contextName) + ": '" + std::string(qname) + "'."));
    }
  }
}

} // namespace XML_Lib
