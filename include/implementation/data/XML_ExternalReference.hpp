#pragma once

#include <string>

namespace XML_Lib {

struct XML_ExternalReference
{
  // Reference types
protected:
  enum class Type { base = 0, systemID, publicID };

public:
  // ExternalReference Error
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("ExternalReference Error: " + message) {}
  };
  // Constructors/Destructors
  XML_ExternalReference(const std::string &referenceType,
    const std::string &systemID = "",
    const std::string publicID = "")
    : systemID(systemID), publicID(publicID)
  {
    if (referenceType == "SYSTEM") {
      type = Type::systemID;
    } else if (referenceType == "PUBLIC") {
      type = Type::publicID;
    } else if (referenceType == "") {
      type = Type::base;
    } else {
      throw Error("Invalid reference type passed.");
    }
  }
  XML_ExternalReference(const XML_ExternalReference &other) = default;
  XML_ExternalReference &operator=(const XML_ExternalReference &other) = default;
  XML_ExternalReference(XML_ExternalReference &&other) = default;
  XML_ExternalReference &operator=(XML_ExternalReference &&other) = default;
  ~XML_ExternalReference() = default;
  // Get external reference details
  const std::string getType() const
  {
    if (type == Type::publicID) {
      return ("PUBLIC");
    } else if (type == Type::systemID) {
      return ("SYSTEM");
    } else if (type == Type::base) {
      return ("");
    }
    throw Error("Invalid external reference type.");
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