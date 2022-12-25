#pragma once
// =======
// C++ STL
// =======
#include <memory>
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ====================
// FORWARD DECLARATIONS
// ====================
#include "XML_Forward.hpp"
// ================
// CLASS DEFINITION
// ================
class XML
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  XML();
  XML(const XML &other) = delete;
  XML &operator=(const XML &other) = delete;
  XML(XML &&other) = delete;
  XML &operator=(XML &&other) = delete;
  ~XML();
  // ==============
  // PUBLIC METHODS
  // ==============
  // Get reference to DTD XNode
  XNode &dtd();
  // Get reference to proglog XNode
  XNode &prolog();
  // Get reference to declaration XNode
  XNode &declaration();
  // Get reference to root element XNode
  XNode &root();
  // Validate XML
  void validate();
  // Return version string
  std::string version();
  // Parse XML source
  void parse(ISource &source);
  void parse(ISource &&source);
  // Stringify XML object to destination text 
  void stringify(IDestination &destination);
  void stringify(IDestination &&destination);
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
  const std::unique_ptr<XML_Impl> m_implementation;
};
}// namespace XML_Lib
