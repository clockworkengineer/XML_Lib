#pragma once

namespace XML_Lib {

struct CDATA final : Variant
{
  // Constructors/Destructors
  explicit CDATA(const std::string_view &cdata) : Variant(Type::cdata), cdata(cdata) {}
  CDATA(const CDATA &other) = delete;
  CDATA &operator=(const CDATA &other) = delete;
  CDATA(CDATA &&other) = default;
  CDATA &operator=(CDATA &&other) = default;
  ~CDATA() override = default;
  // Return reference to cdata
  [[nodiscard]] std::string_view value() const { return cdata; }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override { return cdata; }
  
private:
  std::string cdata;
};
}// namespace XML_Lib