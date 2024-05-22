#pragma once

#include <string>

namespace XML_Lib {

struct XCDATA final : Variant
{
  // Constructors/Destructors
  explicit XCDATA(const std::string &cdata) : Variant(Type::cdata), cdata(cdata) {}
  XCDATA(const XCDATA &other) = delete;
  XCDATA &operator=(const XCDATA &other) = delete;
  XCDATA(XCDATA &&other) = default;
  XCDATA &operator=(XCDATA &&other) = default;
  ~XCDATA() override = default;
  // Return reference to cdata
  [[nodiscard]] const std::string &CDATA() const { return cdata; }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override { return cdata; }
  
private:
  std::string cdata;
};
}// namespace XML_Lib