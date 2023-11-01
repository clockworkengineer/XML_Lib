#pragma once

#include <memory>

namespace XML_Lib {
// ====================
// FORWARD DECLARATIONS
// ====================
#include "DTD_Forward.hpp"
// ================
// CLASS DEFINITION
// ================
class DTD
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  explicit DTD(XNode &xNode);
  DTD() = delete;
  DTD(const DTD &other) = delete;
  DTD &operator=(const DTD &other) = delete;
  DTD(DTD &&other) = delete;
  DTD &operator=(DTD &&other) = delete;
  ~DTD();
  // ==============
  // PUBLIC METHODS
  // ==============
  // Parse XML DTD from source
  void parse(ISource &source);
  // Stringify XML DTD to text destination
  void stringify(IDestination &destination);
  // ================
  // PUBLIC VARIABLES
  // ================
private:
  // ===========================
  // PRIVATE TYPES AND CONSTANTS
  // ===========================
  // ===========================================
  // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
  // ===========================================
  // ===============
  // PRIVATE METHODS
  // ===============
  // =================
  // PRIVATE VARIABLES
  // =================
  const std::unique_ptr<DTD_Impl> m_implementation;
};
}// namespace XML_Lib
