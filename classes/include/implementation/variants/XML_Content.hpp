#pragma once

namespace XML_Lib {

struct Content final : Variant
{
  // Constructors/Destructors
  explicit Content(const std::string_view &content, const bool whiteSpaceDefault = true) : Variant(Type::content), xmlContent(content), whiteSpace(whiteSpaceDefault) {}
  XML_LIB_NO_COPY_MOVE_DTOR(Content);
  // Get reference to content string
  [[nodiscard]] std::string value() const { return xmlContent; }
  // Add to content
  void addContent(const std::string_view &content) { xmlContent += content; }
  // Is content all whitespace
  [[nodiscard]] bool isWhiteSpace() const { return whiteSpace; }
  // Set whitespace boolean
  void setIsWhiteSpace(const bool isWhiteSpace) { whiteSpace = isWhiteSpace; }
  // Return Variant contents
  [[nodiscard]]  std::string getContents() const override { return xmlContent; }

private:
  std::string xmlContent;
  bool whiteSpace;
};
}// namespace XML_Lib