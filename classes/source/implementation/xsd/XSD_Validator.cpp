#include "XSD_Impl.hpp"
#include "ValidatorPimpl.tpp"

namespace XML_Lib {

template class ValidatorPimpl<XSD_Impl>;

XSD_Validator::XSD_Validator(Node &xNode) : ValidatorPimpl<XSD_Impl>(xNode) {}
XSD_Validator::~XSD_Validator() = default;

}// namespace XML_Lib
