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
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class XML_Impl;
class ISource;
class IDestination;
struct XMLNodeElement;
struct XMLNodeProlog;
struct XMLNodeDeclaration;
struct XMLNodeDTD;
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
  XMLNodeDTD &dtd();
  XMLNodeProlog &prolog();
  XMLNodeDeclaration &declaration();
  XMLNodeElement &root();
  void validate();
  std::string version();
  void parse(ISource &source);
  void parse(ISource &&source);
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
