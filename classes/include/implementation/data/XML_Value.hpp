#pragma once

namespace XML_Lib {

struct XMLValue
{
  // Constructors/Destructors
  explicit XMLValue(std::string unparsed = "", std::string parsed = "", const char quote = '\"')
    : unparsed(std::move(unparsed)), parsed(std::move(parsed)), quote(quote)
  {}
  XMLValue() = delete;
  XMLValue(const XMLValue &other) = default;
  virtual XMLValue &operator=(const XMLValue &other) = default;
  XMLValue(XMLValue &&other) = default;
  XMLValue &operator=(XMLValue &&other) = default;
  virtual ~XMLValue() = default;
  // Is a reference value?
  [[nodiscard]] bool isReference() const { return unparsed[0] == '&' && unparsed.back() == ';'; }
  [[nodiscard]] bool isEntityReference() const { return isReference() && unparsed[1] != '#'; }
  [[nodiscard]] bool isCharacterReference() const { return isReference() && unparsed[1] == '#'; }
  // Get value
  [[nodiscard]] const std::string &getUnparsed() const { return unparsed; }
  [[nodiscard]] const std::string &getParsed() const { return parsed; }
  [[nodiscard]] char getQuote() const { return quote; }

protected:
   void setValue(const std::string &str1, const std::string &str2)
  {
    unparsed = str1;
    parsed = str2;
  }

private:
  // Parsed/Unparsed value
  std::string unparsed{};
  std::string parsed{};
  // Quote used for value
  char quote;
};
}// namespace XML_Lib