#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
struct Node;

// ==========================================================
// Interface for the action events during XML tree traversal
// ==========================================================
class IAction
{
public:
  // =============
  // IAction Error
  // =============
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IAction Error: ").append(message)) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IAction() = default;
  // ============================
  // Node encountered so process
  // ============================
  virtual void onNode([[maybe_unused]] Node &Node){ }
  virtual void onNode([[maybe_unused]] const Node &Node){ }
  // ==================================
  // CDATA Node encountered so process
  // ==================================
  virtual void onCDATA([[maybe_unused]] Node &Node){ }
  virtual void onCDATA([[maybe_unused]] const Node &Node){ }
  // ====================================
  // Comment Node encountered so process
  // ====================================
  virtual void onComment([[maybe_unused]] Node &Node){ }
  virtual void onComment([[maybe_unused]] const Node &Node){ }
  // ====================================
  // Content Node encountered so process
  // ====================================
  virtual void onContent([[maybe_unused]] Node &Node){ }
  virtual void onContent([[maybe_unused]] const Node &Node){ }
  // ========================================
  // Declaration Node encountered so process
  // ========================================
  virtual void onDeclaration([[maybe_unused]] Node &Node){ }
  virtual void onDeclaration([[maybe_unused]] const Node &Node){ }
  // ================================
  // DTD_Validator Node encountered so process
  // ================================
  virtual void onDTD([[maybe_unused]] Node &Node){ }
  virtual void onDTD([[maybe_unused]] const Node &Node){ }
  // ====================================
  // Element Node encountered so process
  // ====================================
  virtual void onElement([[maybe_unused]] Node &Node){ }
  virtual void onElement([[maybe_unused]] const Node &Node){ }
  // =============================================
  // Entity reference Node encountered so process
  // =============================================
  virtual void onEntityReference([[maybe_unused]] Node &Node){ }
  virtual void onEntityReference([[maybe_unused]] const Node &Node){ }
  // ===============================
  // PI Node encountered so process
  // ===============================
  virtual void onPI([[maybe_unused]] Node &Node){ }
  virtual void onPI([[maybe_unused]] const Node &Node){ }
  // ===================================
  // Prolog Node encountered so process
  // ===================================
  virtual void onProlog([[maybe_unused]] Node &Node){ }
  virtual void onProlog([[maybe_unused]] const Node &Node){ }
  // =================================
  // Root Node encountered so process
  // =================================
  virtual void onRoot([[maybe_unused]] Node &Node){ }
  virtual void onRoot([[maybe_unused]] const Node &Node){ }
  // =================================
  // Self Node encountered so process
  // =================================
  virtual void onSelf([[maybe_unused]] Node &Node){ }
  virtual void onSelf([[maybe_unused]] const Node &Node){ }

};
}// namespace XML_Lib