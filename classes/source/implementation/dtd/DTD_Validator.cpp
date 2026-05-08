#include "DTD_Impl.hpp"
#include "ValidatorPimpl.tpp"

namespace XML_Lib {

template class ValidatorPimpl<DTD_Impl>;

DTD_Validator::DTD_Validator(Node &xNode) : ValidatorPimpl<DTD_Impl>(xNode) {}
DTD_Validator::~DTD_Validator() = default;

}// namespace XML_Lib
