#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class DTD_Impl;
struct XNode;

class DTD_Validator final : public IValidator
{
public:
  // Constructors/Destructors
  explicit DTD_Validator(XNode &xNode);
  DTD_Validator() = delete;
  DTD_Validator(const DTD_Validator &other) = delete;
  DTD_Validator &operator=(const DTD_Validator &other) = delete;
  DTD_Validator(DTD_Validator &&other) = delete;
  DTD_Validator &operator=(DTD_Validator &&other) = delete;
  ~DTD_Validator() override;
  // Parse XML DTD_Validator from source
  void parse(ISource &source) override;
  // Stringify XML DTD_Validator to text destination
  void stringify(IDestination &destination) override;
  // Validate XML against DTD_Validator
  void validate(const XNode &xNode) override;

private:
  const std::unique_ptr<DTD_Impl> implementation;
};
}// namespace XML_Lib
