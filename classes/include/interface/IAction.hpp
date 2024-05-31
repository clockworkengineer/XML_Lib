
#pragma once

#include <stdexcept>
#include <string>

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
  virtual void onXNode([[maybe_unused]] XNode &XNode) = 0;
  virtual void onXNode([[maybe_unused]] const XNode &XNode) = 0;
  // ==================================
  // CDATA XNode encountered so process
  // ==================================
  virtual void onCDATA([[maybe_unused]] XNode &XNode) = 0;
  virtual void onCDATA([[maybe_unused]] const XNode &XNode) = 0;
  // ====================================
  // Comment XNode encountered so process
  // ====================================
  virtual void onComment([[maybe_unused]] XNode &XNode) = 0;
  virtual void onComment([[maybe_unused]] const XNode &XNode) = 0;
  // ====================================
  // Content XNode encountered so process
  // ====================================
  virtual void onContent([[maybe_unused]] XNode &XNode) = 0;
  virtual void onContent([[maybe_unused]] const XNode &XNode) = 0;
  // ========================================
  // Declaration XNode encountered so process
  // ========================================
  virtual void onDeclaration([[maybe_unused]] XNode &XNode) = 0;
  virtual void onDeclaration([[maybe_unused]] const XNode &XNode) = 0;
  // ================================
  // DTD XNode encountered so process
  // ================================
  virtual void onDTD([[maybe_unused]] XNode &XNode) = 0;
  virtual void onDTD([[maybe_unused]] const XNode &XNode) = 0;
  // ====================================
  // Element XNode encountered so process
  // ====================================
  virtual void onElement([[maybe_unused]] XNode &XNode) = 0;
  virtual void onElement([[maybe_unused]] const XNode &XNode) = 0;
  // =============================================
  // Entity reference XNode encountered so process
  // =============================================
  virtual void onEntityReference([[maybe_unused]] XNode &XNode) = 0;
  virtual void onEntityReference([[maybe_unused]] const XNode &XNode) = 0;
  // ===============================
  // PI XNode encountered so process
  // ===============================
  virtual void onPI([[maybe_unused]] XNode &XNode) = 0;
  virtual void onPI([[maybe_unused]] const XNode &XNode) = 0;
  // ===================================
  // Prolog XNode encountered so process
  // ===================================
  virtual void onProlog([[maybe_unused]] XNode &XNode) = 0;
  virtual void onProlog([[maybe_unused]] const XNode &XNode) = 0;
  // =================================
  // Root XNode encountered so process
  // =================================
  virtual void onRoot([[maybe_unused]] XNode &XNode) = 0;
  virtual void onRoot([[maybe_unused]] const XNode &XNode) = 0;
  // =================================
  // Self XNode encountered so process
  // =================================
  virtual void onSelf([[maybe_unused]] XNode &XNode) = 0;
  virtual void onSelf([[maybe_unused]] const XNode &XNode) = 0;

};
}// namespace XML_Lib