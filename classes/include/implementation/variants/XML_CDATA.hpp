#pragma once

namespace XML_Lib {

struct CDATA final : Variant
{
  // Constructors/Destructors
  explicit CDATA(std::string cdata) : Variant(Type::cdata), cdata(std::move(cdata)) {}
  CDATA(const CDATA &other) = delete;
  CDATA &operator=(const CDATA &other) = delete;
  CDATA(CDATA &&other) = default;
  CDATA &operator=(CDATA &&other) = default;
  ~CDATA() override = default;
  // Return reference to cdata
  [[nodiscard]] const std::string &value() const { return cdata; }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override { return cdata; }
  
private:
  std::string cdata;
};
}// namespace XML_Lib