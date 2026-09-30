#include "XSD_Impl.hpp"
#include "XSD_Schema.hpp"
#include "ValidatorPimpl.tpp"

namespace XML_Lib {

template class ValidatorPimpl<XSD_Impl>;

XSD_Validator::XSD_Validator(Node &xNode) : ValidatorPimpl<XSD_Impl>(xNode) {}

XSD_Validator::XSD_Validator(Node &xNode, const XSD_Schema &schema)
  : ValidatorPimpl<XSD_Impl>(xNode)
{
  implementation = std::make_unique<XSD_Impl>(xNode, schema.definition());
}

XSD_Validator::~XSD_Validator() = default;

}// namespace XML_Lib
