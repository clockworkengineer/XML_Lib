#pragma once

#include <string>

namespace XML_Lib {

struct XMLExternalReference
{
  // Constructors/Destructors
  XMLExternalReference(const std::string &type, const std::string &systemID = "", const std::string publicID = "")
    : m_type(type), m_systemID(systemID), m_publicID(publicID)
  {}
  XMLExternalReference(const XMLExternalReference &other) = default;
  XMLExternalReference &operator=(const XMLExternalReference &other) = default;
  XMLExternalReference(XMLExternalReference &&other) = default;
  XMLExternalReference &operator=(XMLExternalReference &&other) = default;
  ~XMLExternalReference() = default;
  // Get external reference details
  const std::string &getType() const { return (m_type); }
  const std::string &getSystemID() const { return (m_systemID); }
  const std::string &getPublicID() const { return (m_publicID); }

private:
  // Reference type and ids
  std::string m_type{};
  std::string m_systemID{};
  std::string m_publicID{};
};
}// namespace XML_Lib