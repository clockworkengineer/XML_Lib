#pragma once

#include <string>
#include <utility>

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
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("ExternalReference Error: " + message) {}
  };
  // Constructors/Destructors
  explicit XMLExternalReference(const std::string &referenceType,
    std::string systemID = "",
    std::string publicID = "")
    : systemID(std::move(systemID)), publicID(std::move(publicID))
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
  XMLExternalReference() = delete;
  XMLExternalReference(const XMLExternalReference &other) = default;
  XMLExternalReference &operator=(const XMLExternalReference &other) = default;
  XMLExternalReference(XMLExternalReference &&other) = default;
  XMLExternalReference &operator=(XMLExternalReference &&other) = default;
  ~XMLExternalReference() = default;
  // Get external reference details
  [[nodiscard]] std::string getType() const
  {
    if (type == Type::publicID) { return kPublicID; }
    if (type == Type::systemID) { return kSystemID; }
    throw Error("External reference type not set.");
  }
  // Check which reference is set
  [[nodiscard]] bool isPublic() const { return type == Type::publicID; }
  [[nodiscard]] bool isSystem() const { return type == Type::systemID; }
  // Get reference values
  [[nodiscard]] const std::string &getSystemID() const
  {
    if (type != Type::base) { return systemID; }
    throw Error("External reference has no system value set.");
  }
  [[nodiscard]] const std::string &getPublicID() const
  {
    if (type == Type::publicID) { return publicID; }
    throw Error("External reference is not public.");
  }

private:
  // Reference type and ids
  Type type{};
  std::string systemID{};
  std::string publicID{};
};
}// namespace XML_Lib