#pragma once

namespace XML_Lib {

struct Content final : Variant
{
  // Constructors/Destructors
  explicit Content(std::string content, const bool whiteSpaceDefault = true) : Variant(Type::content), xmlContent(std::move(content)), whiteSpace(whiteSpaceDefault) {}
  Content(const Content &other) = delete;
  Content &operator=(const Content &other) = delete;
  Content(Content &&other) = default;
  Content &operator=(Content &&other) = default;
  ~Content() override = default;
  // Get reference to content string
  [[nodiscard]] std::string value() const { return xmlContent; }
  // Add to content
  void addContent(const std::string &content) { xmlContent += content; }
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