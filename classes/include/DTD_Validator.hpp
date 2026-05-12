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

/// @brief DTD validator.
///
/// Validates an XML document tree against an embedded or external DTD definition.
/// Pass an instance of this class to `XML::validate()`.
///
/// @note Non-copyable, non-movable.
class DTD_Validator final : public ValidatorPimpl<DTD_Impl>
{
public:
  /// @brief Construct a DTD validator bound to @p xNode (the DTD node in the document tree).
  explicit DTD_Validator(Node &xNode);
  DTD_Validator() = delete;
  DTD_Validator(const DTD_Validator &other) = delete;
  DTD_Validator &operator=(const DTD_Validator &other) = delete;
  DTD_Validator(DTD_Validator &&other) = delete;
  DTD_Validator &operator=(DTD_Validator &&other) = delete;
  /// @brief Destructor — out-of-line so DTD_Impl can remain an incomplete type in this header.
  ~DTD_Validator() override;
};
}// namespace XML_Lib

#endif// XML_LIB_ENABLE_DTD
