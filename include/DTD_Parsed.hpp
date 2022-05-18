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
#include "IXMLEntityMapper.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    struct DTD_Parsed
    {
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        DTD_Parsed(IXMLEntityMapper &entityMapper) : m_entityMapper(entityMapper)
        {
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        uint16_t getType() const
        {
            return (m_type);
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        void setType(uint16_t type)
        {
            m_type = type;
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        std::string &getRootName()
        {
            return (m_name);
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        void setRootName(const std::string &name)
        {
            m_name = name;
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        XMLExternalReference &getExternalReference()
        {
            return (m_external);
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        void setExternalReference(const XMLExternalReference &reference)
        {
            m_external = reference;
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        bool isElementPresent(const std::string &elementName) const
        {
            return (m_elements.find(elementName) != m_elements.end());
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        DTDElement &getElement(const std::string &elementName)
        {
            return (m_elements[elementName]);
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        void addElement(const std::string &elementName, const DTDElement &element)
        {
            m_elements.emplace(elementName, element);
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        [[nodiscard]] long getElementCount() const
        {
            return (static_cast<long>(m_elements.size()));
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        bool isEntityPresent(const std::string &entityName) const
        {
            return (m_entityMapper.isPresent(entityName));
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        XMLEntityMapping &getEntity(const std::string &entityName)
        {
            return (m_entityMapper.get(entityName));
        }
        /// <summary>
        ///
        /// </summary>
        /// <param name=""></param>
        XMLExternalReference &getNotation(const std::string &notationName)
        {
            return (m_notations[notationName]);
        }

        std::unordered_map<std::string, XMLExternalReference> m_notations;

        IXMLEntityMapper &m_entityMapper;

    private:
        uint16_t m_type{};
        std::string m_name;
        XMLExternalReference m_external;
        std::unordered_map<std::string, DTDElement> m_elements;
    };

} // namespace XMLLib