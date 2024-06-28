#pragma once

namespace XML_Lib {

struct XDeclaration final : Variant
{
  // Constructors/Destructors
  XDeclaration(std::string version, const std::string &encoding, const std::string &standalone)
    : Variant(Type::declaration), xmlVersion(std::move(version)), xmlEncoding(encoding), xmlStandalone(standalone)
  {}
  XDeclaration(const XDeclaration &other) = delete;
  XDeclaration &operator=(const XDeclaration &other) = delete;
  XDeclaration(XDeclaration &&other) = default;
  XDeclaration &operator=(XDeclaration &&other) = default;
  ~XDeclaration() override = default;
  // Get reference to declaration attributes
  [[nodiscard]] const std::string &version() const { return xmlVersion; }
  [[nodiscard]] const std::string &encoding() const { return xmlEncoding; }
  [[nodiscard]] const std::string &standalone() const { return xmlStandalone; }
  // Set declaration attributes
  void setVersion(const std::string &version) { xmlVersion = version; }
  void setEncoding(const std::string &encoding) { xmlEncoding = encoding; }
  void setStandalone(const std::string &standalone) { xmlStandalone = standalone; }

private:
  std::string xmlVersion;
  std::string xmlEncoding;
  std::string xmlStandalone;
};
}// namespace XML_Lib