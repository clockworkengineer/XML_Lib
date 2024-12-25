#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
struct XNode;

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
    explicit Error(const std::string &message) : std::runtime_error("IAction Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IAction() = default;
  // ============================
  // XNode encountered so process
  // ============================
  virtual void onXNode([[maybe_unused]] XNode &XNode){ }
  virtual void onXNode([[maybe_unused]] const XNode &XNode){ }
  // ==================================
  // CDATA XNode encountered so process
  // ==================================
  virtual void onCDATA([[maybe_unused]] XNode &XNode){ }
  virtual void onCDATA([[maybe_unused]] const XNode &XNode){ }
  // ====================================
  // Comment XNode encountered so process
  // ====================================
  virtual void onComment([[maybe_unused]] XNode &XNode){ }
  virtual void onComment([[maybe_unused]] const XNode &XNode){ }
  // ====================================
  // Content XNode encountered so process
  // ====================================
  virtual void onContent([[maybe_unused]] XNode &XNode){ }
  virtual void onContent([[maybe_unused]] const XNode &XNode){ }
  // ========================================
  // Declaration XNode encountered so process
  // ========================================
  virtual void onDeclaration([[maybe_unused]] XNode &XNode){ }
  virtual void onDeclaration([[maybe_unused]] const XNode &XNode){ }
  // ================================
  // DTD_Validator XNode encountered so process
  // ================================
  virtual void onDTD([[maybe_unused]] XNode &XNode){ }
  virtual void onDTD([[maybe_unused]] const XNode &XNode){ }
  // ====================================
  // Element XNode encountered so process
  // ====================================
  virtual void onElement([[maybe_unused]] XNode &XNode){ }
  virtual void onElement([[maybe_unused]] const XNode &XNode){ }
  // =============================================
  // Entity reference XNode encountered so process
  // =============================================
  virtual void onEntityReference([[maybe_unused]] XNode &XNode){ }
  virtual void onEntityReference([[maybe_unused]] const XNode &XNode){ }
  // ===============================
  // PI XNode encountered so process
  // ===============================
  virtual void onPI([[maybe_unused]] XNode &XNode){ }
  virtual void onPI([[maybe_unused]] const XNode &XNode){ }
  // ===================================
  // Prolog XNode encountered so process
  // ===================================
  virtual void onProlog([[maybe_unused]] XNode &XNode){ }
  virtual void onProlog([[maybe_unused]] const XNode &XNode){ }
  // =================================
  // Root XNode encountered so process
  // =================================
  virtual void onRoot([[maybe_unused]] XNode &XNode){ }
  virtual void onRoot([[maybe_unused]] const XNode &XNode){ }
  // =================================
  // Self XNode encountered so process
  // =================================
  virtual void onSelf([[maybe_unused]] XNode &XNode){ }
  virtual void onSelf([[maybe_unused]] const XNode &XNode){ }

};
}// namespace XML_Lib