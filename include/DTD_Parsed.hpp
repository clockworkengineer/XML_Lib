#pragma once
//
// C++ STL
//
#include <string>
#include <unordered_map>
//
// XML
//
#include "XML_Types.hpp"
#include "DTD_Element.hpp"
#include "XML_EntityMapper.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    struct DTDParsed
    {
        explicit DTDParsed(IXMLEntityMapper &entityMapper) : m_entityMapper(entityMapper)
        {
        }
        uint16_t getType() const
        {
            return (m_type);
        }
        void setType(uint16_t type)
        {
            m_type = type;
        }
        const std::string getRootName() const
        {
            return (m_name);
        }
        void setRootName(const std::string &name)
        {
            m_name = name;
        }
        XMLExternalReference getExternalReference() const
        {
            return (m_external);
        }
        void setExternalReference(const XMLExternalReference &reference)
        {
            m_external = reference;
        }
        bool isElementPresent(const std::string &elementName) const
        {
            return (m_elements.find(elementName) != m_elements.end());
        }
        DTDElement &getElement(const std::string &elementName)
        {
            return (m_elements[elementName]);
        }
        void addElement(const std::string &elementName, const DTDElement &element)
        {
            m_elements.emplace(elementName, element);
        }
        [[nodiscard]] long getElementCount() const
        {
            return (static_cast<long>(m_elements.size()));
        }
        XMLExternalReference &getNotation(const std::string &notationName)
        {
            return (m_notations[notationName]);
        }
        void addNotation(const std::string &notationName, const XMLExternalReference &notation)
        {
            m_notations.emplace(notationName, notation);
        }
        [[nodiscard]] long getNotationCount(const std::string &notationName) const
        {
            return (static_cast<long>(m_notations.count(notationName)));
        }
        long getLineCount() const
        {
            return (m_lineCount);
        }
        void setLineCount(long lineCount)
        {
            m_lineCount = lineCount;
        }
        IXMLEntityMapper &m_entityMapper;

    private:
        uint16_t m_type{};
        long m_lineCount{};
        std::string m_name;
        XMLExternalReference m_external;
        std::unordered_map<std::string, DTDElement> m_elements;
        std::unordered_map<std::string, XMLExternalReference> m_notations;
    };
} // namespace XMLLib