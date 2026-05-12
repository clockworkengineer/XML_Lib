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

/// @brief XSD schema validator.
///
/// Validates an XML document tree against an XSD schema provided as inline text or a file path.
/// Pass an instance to `XML::validate(const std::string_view &)`.
///
/// @note Non-copyable, non-movable.
class XSD_Validator final : public ValidatorPimpl<XSD_Impl>
{
public:
  /// @brief Construct an XSD validator bound to @p xNode (the document root node).
  explicit XSD_Validator(Node &xNode);
  XSD_Validator() = delete;
  XSD_Validator(const XSD_Validator &other) = delete;
  XSD_Validator &operator=(const XSD_Validator &other) = delete;
  XSD_Validator(XSD_Validator &&other) = delete;
  XSD_Validator &operator=(XSD_Validator &&other) = delete;
  /// @brief Destructor — out-of-line so XSD_Impl can remain an incomplete type in this header.
  ~XSD_Validator() override;
};

}// namespace XML_Lib

#endif// XML_LIB_ENABLE_XSD
