#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class XPath_Impl;
struct Node;

class XPath
{
public:
  // =====================
  // XPath Error
  // =====================
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("XPath Error: ").append(message))
    {}
  };

  // ========================
  // Constructors/Destructors
  // ========================
  explicit XPath(const Node &root);
  XPath() = delete;
  XPath(const XPath &) = delete;
  XPath &operator=(const XPath &) = delete;
  XPath(XPath &&) = delete;
  XPath &operator=(XPath &&) = delete;
  ~XPath();

  // ==================================
  // Evaluate an XPath 1.0 expression.
  // Returns pointers into existing Node tree (do not store beyond XML lifetime).
  // ==================================
  [[nodiscard]] std::vector<const Node *> evaluate(std::string_view expression) const;

  // ==========================================
  // Convenience wrappers — string/bool/number
  // ==========================================
  [[nodiscard]] std::string evaluateString(std::string_view expression) const;
  [[nodiscard]] bool evaluateBool(std::string_view expression) const;
  [[nodiscard]] double evaluateNumber(std::string_view expression) const;

private:
  const std::unique_ptr<XPath_Impl> implementation;
};

}// namespace XML_Lib
