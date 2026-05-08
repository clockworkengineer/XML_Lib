#pragma once

#include "IValidator.hpp"

#include <memory>

namespace XML_Lib {

class ISource;
class IDestination;

// Pimpl forwarding template shared by DTD_Validator and XSD_Validator.
// Method bodies live in ValidatorPimpl.tpp and are explicitly instantiated
// in each validator's .cpp file where Impl is a complete type.
template<typename Impl>
class ValidatorPimpl : public IValidator
{
public:
  explicit ValidatorPimpl(Node &xNode);
  ValidatorPimpl() = delete;
  ValidatorPimpl(const ValidatorPimpl &) = delete;
  ValidatorPimpl &operator=(const ValidatorPimpl &) = delete;
  ValidatorPimpl(ValidatorPimpl &&) = delete;
  ValidatorPimpl &operator=(ValidatorPimpl &&) = delete;
  ~ValidatorPimpl() override;
  void parse(ISource &source) override;
  void stringify(IDestination &dest) override;
  void validate(const Node &xNode) override;
protected:
  std::unique_ptr<Impl> implementation;
};

}// namespace XML_Lib
