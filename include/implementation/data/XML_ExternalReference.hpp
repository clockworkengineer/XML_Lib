#pragma once

#include <string>

namespace XML_Lib {

struct XML_ExternalReference
{
  // Constructors/Destructors
  XML_ExternalReference(const std::string &type, const std::string &systemID = "", const std::string publicID = "")
    : type(type), systemID(systemID), publicID(publicID)
  {}
  XML_ExternalReference(const XML_ExternalReference &other) = default;
  XML_ExternalReference &operator=(const XML_ExternalReference &other) = default;
  XML_ExternalReference(XML_ExternalReference &&other) = default;
  XML_ExternalReference &operator=(XML_ExternalReference &&other) = default;
  ~XML_ExternalReference() = default;
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