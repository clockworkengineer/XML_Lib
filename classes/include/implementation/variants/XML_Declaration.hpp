#pragma once

#include <utility>

namespace XML_Lib {

struct Declaration final : Variant
{
  // Constructors/Destructors
  Declaration(std::string version, std::string encoding, std::string standalone)
    : Variant(Type::declaration), xmlVersion(std::move(version)), xmlEncoding(std::move(encoding)), xmlStandalone(std::move(standalone))
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
  void setVersion(const std::string &version) { xmlVersion = version; }
  void setEncoding(const std::string &encoding) { xmlEncoding = encoding; }
  void setStandalone(const std::string &standalone) { xmlStandalone = standalone; }

private:
  std::string xmlVersion;
  std::string xmlEncoding;
  std::string xmlStandalone;
};
}// namespace XML_Lib