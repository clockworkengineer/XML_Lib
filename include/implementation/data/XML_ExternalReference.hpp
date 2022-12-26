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
  // Constructors/Destructors
  XMLExternalReference(const std::string &type, const std::string &systemID = "", const std::string publicID = "")
    : type(type), systemID(systemID), publicID(publicID)
  {}
  XMLExternalReference(const XMLExternalReference &other) = default;
  XMLExternalReference &operator=(const XMLExternalReference &other) = default;
  XMLExternalReference(XMLExternalReference &&other) = default;
  XMLExternalReference &operator=(XMLExternalReference &&other) = default;
  ~XMLExternalReference() = default;
  // Reference type and ids
  std::string type{};
  std::string systemID{};
  std::string publicID{};
};
}// namespace XML_Lib