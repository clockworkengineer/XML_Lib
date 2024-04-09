#pragma once

#include <string>

namespace XML_Lib {

struct XContent : Variant
{
  // Constructors/Destructors
  explicit XContent(bool whiteSpaceDefault = true) : Variant(Variant::Type::content), whiteSpace(whiteSpaceDefault) {}
  XContent(const XContent &other) = delete;
  XContent &operator=(const XContent &other) = delete;
  XContent(XContent &&other) = default;
  XContent &operator=(XContent &&other) = default;
  ~XContent() = default;
  // Get reference to content string
  [[nodiscard]] std::string getContent() const { return (xmlContent); }
  // Add to content
  void addContent(const std::string &content) { xmlContent += content; }
  // Is content all whitespace
  [[nodiscard]] bool isWhiteSpace() const { return (whiteSpace); }
  // Set whitespace boolean
  void setIsWhiteSpace(bool isWhiteSpace) { whiteSpace = isWhiteSpace; }
  // Return Variant contents
  [[nodiscard]] virtual std::string getContents() const { return (xmlContent); }

private:
  std::string xmlContent;
  bool whiteSpace;
};
}// namespace XML_Lib