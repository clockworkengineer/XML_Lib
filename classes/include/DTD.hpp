#pragma once

#include <regex>

#include "XML_Interfaces.hpp"

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class DTD_Impl;
struct XNode;

class DTD final : public IValidator
{
public:
  // Constructors/Destructors
  explicit DTD(XNode &xNode);
  DTD() = delete;
  DTD(const DTD &other) = delete;
  DTD &operator=(const DTD &other) = delete;
  DTD(DTD &&other) = delete;
  DTD &operator=(DTD &&other) = delete;
  ~DTD() override;
  // Parse XML DTD from source
  void parse(ISource &source) override;
  // Stringify XML DTD to text destination
  void stringify(IDestination &destination) override;
  // Validate XML against DTD
  void validate(const XNode &xNode) override;

private:
  const std::unique_ptr<DTD_Impl> implementation;
};
}// namespace XML_Lib
