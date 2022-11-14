#pragma once
// =======
// C++ STL
// =======
#include <memory>
// =================
// LIBRARY NAMESPACE
// =================
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
  explicit DTD(XMLNodeDTD &xmlNodeDTD);
  DTD() = default;
  DTD(const DTD &other) = delete;
  DTD &operator=(const DTD &other) = delete;
  DTD(DTD &&other) = delete;
  DTD &operator=(DTD &&other) = delete;
  ~DTD();
  // ==============
  // PUBLIC METHODS
  // ==============
  void parse(ISource &source);
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
  std::unique_ptr<DTD_Impl> m_implementation;
};
}// namespace XML_Lib
