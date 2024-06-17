#pragma once

#include <string>
#include <utility>

namespace XML_Lib {

struct XCDATA final : Variant
{
  // Constructors/Destructors
  explicit XCDATA(std::string cdata) : Variant(Type::cdata), cdata(std::move(cdata)) {}
  XCDATA(const XCDATA &other) = delete;
  XCDATA &operator=(const XCDATA &other) = delete;
  XCDATA(XCDATA &&other) = default;
  XCDATA &operator=(XCDATA &&other) = default;
  ~XCDATA() override = default;
  // Return reference to cdata
  [[nodiscard]] const std::string &value() const { return cdata; }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override { return cdata; }
  
private:
  std::string cdata;
};
}// namespace XML_Lib