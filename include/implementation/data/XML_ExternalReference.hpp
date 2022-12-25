#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ======================
// XML External reference
// ======================
struct XMLExternalReference
{
  XMLExternalReference(const std::string &type, const std::string &systemID = "", const std::string publicID = "")
    : type(type), systemID(systemID), publicID(publicID)
  {}
  std::string type{};
  std::string systemID{};
  std::string publicID{};
};
}// namespace XML_Lib