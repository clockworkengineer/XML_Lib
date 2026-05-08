#pragma once

#if defined(XML_LIB_ENABLE_XSD)

#include "implementation/ValidatorPimpl.hpp"

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class XSD_Impl;

// Suppress implicit instantiation in every TU that includes this header;
// the explicit instantiation lives in XSD_Validator.cpp where XSD_Impl is complete.
extern template class ValidatorPimpl<XSD_Impl>;

class XSD_Validator final : public ValidatorPimpl<XSD_Impl>
{
public:
  // Constructors/Destructors
  explicit XSD_Validator(Node &xNode);
  XSD_Validator() = delete;
  XSD_Validator(const XSD_Validator &other) = delete;
  XSD_Validator &operator=(const XSD_Validator &other) = delete;
  XSD_Validator(XSD_Validator &&other) = delete;
  XSD_Validator &operator=(XSD_Validator &&other) = delete;
  // Out-of-line: XSD_Impl must be complete at the definition site (.cpp)
  ~XSD_Validator() override;
};

}// namespace XML_Lib

#endif// XML_LIB_ENABLE_XSD
