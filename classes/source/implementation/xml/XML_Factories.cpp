#include "XML_Factories.hpp"
#include "implementation/entity/XML_EntityMapper.hpp"
#include "implementation/parser/Default_Parser.hpp"
#if defined(XML_LIB_ENABLE_STRINGIFY)
#include "implementation/stringify/Default_Stringify.hpp"
#endif
#include "implementation/ValidatorRegistry.hpp"
#if defined(XML_LIB_ENABLE_XPATH)
#include "implementation/xpath/DefaultXPathEngine.hpp"
#endif

namespace XML_Lib {

std::unique_ptr<IEntityMapper> createDefaultEntityMapper()
{
  return std::make_unique<XML_EntityMapper>();
}

std::unique_ptr<IValidatingParser> createDefaultParser(IEntityMapper &entityMapper)
{
  return std::make_unique<Default_Parser>(entityMapper);
}

#if defined(XML_LIB_ENABLE_STRINGIFY)
std::unique_ptr<IStringify> createDefaultStringify()
{
  return std::make_unique<Default_Stringify>();
}
#endif

std::unique_ptr<IValidatorRegistry> createDefaultValidatorRegistry()
{
  return std::make_unique<ValidatorRegistry>();
}

#if defined(XML_LIB_ENABLE_XPATH)
std::unique_ptr<IXPathEngine> createDefaultXPathEngine()
{
  return std::make_unique<DefaultXPathEngine>();
}
#endif

} // namespace XML_Lib
