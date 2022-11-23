#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ==================
// XML Entity mapping
// ==================
struct XMLEntityMapping
{
  explicit XMLEntityMapping(std::string internal) : internal(std::move(internal)) {}
  XMLEntityMapping(std::string internal, XMLExternalReference external, std::string notation)
    : internal(std::move(internal)), external(std::move(external)), notation(std::move(notation))
  {}
  std::string internal{};
  XMLExternalReference external{ "" };
  std::string notation{};
};
}// namespace XML_Lib