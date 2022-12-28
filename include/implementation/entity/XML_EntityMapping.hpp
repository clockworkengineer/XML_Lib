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
  explicit XMLEntityMapping(const std::string &internal) : internal(internal) {}
  std::string internal{};
  XMLExternalReference external{ "" };
  std::string notation{};
};
}// namespace XML_Lib