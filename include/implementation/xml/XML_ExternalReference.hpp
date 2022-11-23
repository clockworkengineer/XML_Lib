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
  explicit XMLExternalReference(std::string type) : type(std::move(type)) {}
  XMLExternalReference(std::string type, std::string systemID, std::string publicID)
    : type(std::move(type)), systemID(std::move(systemID)), publicID(std::move(publicID))
  {}
  std::string type{};
  std::string systemID{};
  std::string publicID{};
};
}