#pragma once
//
// C++ STL
//
#include <string>
#include <unordered_map>
//
//
//
#include "XML_Types.hpp"
#include "DTD_Element.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    struct DTD_Parsed
    {
        DTD_Parsed(IXMLEntityMapper &entityMapper) : m_entityMapper(entityMapper){}
        uint16_t m_type{};
        std::string m_name;
        XMLExternalReference m_external;
        std::unordered_map<std::string, DTDElement> m_elements;
        std::unordered_map<std::string, XMLExternalReference> m_notations;
        IXMLEntityMapper &m_entityMapper;
    };

} // namespace XMLLib