#pragma once

namespace XML_Lib {

struct Declaration final : Variant
{
  // Constructors/Destructors
  Declaration(const std::string_view & version, const std::string_view & encoding, const std::string_view & standalone)
    : Variant(Type::declaration), xmlVersion(version), xmlEncoding(encoding), xmlStandalone(standalone)
  {}
  Declaration(const Declaration &other) = delete;
  Declaration &operator=(const Declaration &other) = delete;
  Declaration(Declaration &&other) = default;
  Declaration &operator=(Declaration &&other) = default;
  ~Declaration() override = default;
  // Get reference to declaration attributes
  [[nodiscard]] const std::string &version() const { return xmlVersion; }
  [[nodiscard]] const std::string &encoding() const { return xmlEncoding; }
  [[nodiscard]] const std::string &standalone() const { return xmlStandalone; }
  // Set declaration attributes
  void setVersion(const std::string_view &version) { xmlVersion = version; }
  void setEncoding(const std::string_view &encoding) { xmlEncoding = encoding; }
  void setStandalone(const std::string_view &standalone) { xmlStandalone = standalone; }

private:
  std::string xmlVersion;
  std::string xmlEncoding;
  std::string xmlStandalone;
};
}// namespace XML_Lib