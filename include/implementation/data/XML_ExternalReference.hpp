#pragma once

#include <string>

namespace XML_Lib {

struct XMLExternalReference
{
  // Reference types
protected:
  enum class Type { base = 0, systemID, publicID };

public:
  // Constants
  static constexpr char kPublicID[] = "PUBLIC";
  static constexpr char kSystemID[] = "SYSTEM";
  // ExternalReference Error
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("ExternalReference Error: " + message) {}
  };
  // Constructors/Destructors
  XMLExternalReference(const std::string &referenceType,
    const std::string &systemID = "",
    const std::string &publicID = "")
    : systemID(systemID), publicID(publicID)
  {
    if (referenceType == kSystemID) {
      type = Type::systemID;
    } else if (referenceType == kPublicID) {
      type = Type::publicID;
    } else if (referenceType == "") {
      type = Type::base;
    } else {
      throw Error("Invalid reference type passed.");
    }
  }
  XMLExternalReference()=delete;
  XMLExternalReference(const XMLExternalReference &other) = default;
  XMLExternalReference &operator=(const XMLExternalReference &other) = default;
  XMLExternalReference(XMLExternalReference &&other) = default;
  XMLExternalReference &operator=(XMLExternalReference &&other) = default;
  ~XMLExternalReference() = default;
  // Get external reference details
  const std::string getType() const
  {
    if (type == Type::publicID) {
      return (kPublicID);
    } else if (type == Type::systemID) {
      return (kSystemID);
    } else {// base
      return ("");
    }
  }

  const std::string &getSystemID() const { return (systemID); }
  const std::string &getPublicID() const { return (publicID); }

private:
  // Reference type and ids
  Type type{};
  std::string systemID{};
  std::string publicID{};
};
}// namespace XML_Lib