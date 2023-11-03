#pragma once

#include <string>

namespace XML_Lib {

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
  // Get external reference details
  const std::string &getType() const { return (type); }
  const std::string &getSystemID() const { return (systemID); }
  const std::string &getPublicID() const { return (publicID); }

private:
  // Reference type and ids
  std::string type{};
  std::string systemID{};
  std::string publicID{};
};
}// namespace XML_Lib