#pragma once

#include <memory>

namespace XML_Lib {

// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class DTD_Impl;
class ISource;
class IDestination;
struct XNode;

class DTD
{
public:
  // Constructors/Destructors
  explicit DTD(XNode &xNode);
  DTD() = delete;
  DTD(const DTD &other) = delete;
  DTD &operator=(const DTD &other) = delete;
  DTD(DTD &&other) = delete;
  DTD &operator=(DTD &&other) = delete;
  ~DTD();

  // Parse XML DTD from source
  void parse(ISource &source);
  // Stringify XML DTD to text destination
  void stringify(IDestination &destination);

private:
  const std::unique_ptr<DTD_Impl> implementation;
};
}// namespace XML_Lib
