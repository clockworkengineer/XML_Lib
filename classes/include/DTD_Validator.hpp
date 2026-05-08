#pragma once

#if defined(XML_LIB_ENABLE_DTD)

#include "implementation/ValidatorPimpl.hpp"

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class DTD_Impl;

// Suppress implicit instantiation in every TU that includes this header;
// the explicit instantiation lives in DTD_Validator.cpp where DTD_Impl is complete.
extern template class ValidatorPimpl<DTD_Impl>;

class DTD_Validator final : public ValidatorPimpl<DTD_Impl>
{
public:
  // Constructors/Destructors
  explicit DTD_Validator(Node &xNode);
  DTD_Validator() = delete;
  DTD_Validator(const DTD_Validator &other) = delete;
  DTD_Validator &operator=(const DTD_Validator &other) = delete;
  DTD_Validator(DTD_Validator &&other) = delete;
  DTD_Validator &operator=(DTD_Validator &&other) = delete;
  // Out-of-line: DTD_Impl must be complete at the definition site (.cpp)
  ~DTD_Validator() override;
};
}// namespace XML_Lib

#endif// XML_LIB_ENABLE_DTD
