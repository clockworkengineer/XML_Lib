#pragma once

#if defined(XML_LIB_ENABLE_XPATH)

#include "interface/IXPathEngine.hpp"
#include "XPath.hpp"

namespace XML_Lib {

/// @brief Default XPath 1.0 engine implementation of IXPathEngine.
class DefaultXPathEngine final : public IXPathEngine
{
public:
  ~DefaultXPathEngine() noexcept override = default;

  [[nodiscard]] std::vector<const Node *> evaluate(const Node &contextNode, std::string_view expression) const override
  {
    XPath xp(contextNode);
    return xp.evaluate(expression);
  }
};

} // namespace XML_Lib

#endif // XML_LIB_ENABLE_XPATH
