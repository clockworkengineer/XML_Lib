#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class XSD_Impl;
struct Node;

class XSD_Validator final : public IValidator
{
public:
  // Constructors/Destructors
  explicit XSD_Validator(Node &xNode);
  XSD_Validator() = delete;
  XSD_Validator(const XSD_Validator &other) = delete;
  XSD_Validator &operator=(const XSD_Validator &other) = delete;
  XSD_Validator(XSD_Validator &&other) = delete;
  XSD_Validator &operator=(XSD_Validator &&other) = delete;
  ~XSD_Validator() override;
  // Parse XSD schema from source
  void parse(ISource &source) override;
  // Stringify XSD schema to text destination
  void stringify(IDestination &destination) override;
  // Validate XML against XSD schema
  void validate(const Node &xNode) override;

private:
  const std::unique_ptr<XSD_Impl> implementation;
};

}// namespace XML_Lib
