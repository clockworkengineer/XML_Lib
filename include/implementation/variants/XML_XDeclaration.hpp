#pragma once

#include <string>

namespace XML_Lib {

struct XDeclaration : Variant
{
  // Constructors/Destructors
  XDeclaration(const std::string &version, const std::string &encoding, const std::string &standalone)
    : Variant(Variant::Type::declaration), xmlVersion(version), xmlEncoding(encoding), xmlStandalone(standalone)
  {}
  XDeclaration(const XDeclaration &other) = delete;
  XDeclaration &operator=(const XDeclaration &other) = delete;
  XDeclaration(XDeclaration &&other) = default;
  XDeclaration &operator=(XDeclaration &&other) = default;
  ~XDeclaration() = default;
  // Get reference to declaration attibutes
  [[nodiscard]] const std::string &version() const { return (xmlVersion); }
  [[nodiscard]] const std::string &encoding() const { return (xmlEncoding); }
  [[nodiscard]] const std::string &standalone() const { return (xmlStandalone); }

private:
  std::string xmlVersion;
  std::string xmlEncoding;
  std::string xmlStandalone;
};
}// namespace XML_Lib