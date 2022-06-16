#pragma once
//
// C++ STL
//
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    class IXMLEntityMapper;
    // ==============
    // XML Node types
    // ==============
    enum class XMLNodeType
    {
        base = 0,
        prolog,
        declaration,
        root,
        self,
        element,
        content,
        entity,
        comment,
        cdata,
        pi,
        dtd
    };
    // ====
    // Base
    // ====
    struct XMLNode
    {
        using Ptr = std::unique_ptr<XMLNode>;
        struct Error : public std::exception
        {
        public:
            explicit Error(std::string errorMessage = "") : errorMessage(std::string("XMLNode Error: ") + errorMessage) {}
            [[nodiscard]] const char *what() const noexcept override
            {
                return (errorMessage.c_str());
            }

        private:
            const std::string errorMessage;
        };
        explicit XMLNode(XMLNodeType nodeType = XMLNodeType::base) : xmlNodeType(nodeType)
        {
        }
        [[nodiscard]] XMLNodeType getNodeType() const
        {
            return (xmlNodeType);
        }
        void setNodeType(XMLNodeType nodeType)
        {
            xmlNodeType = nodeType;
        }
        [[nodiscard]] const std::string getContents() const;
        const XMLNode &operator[](int index) const;
        const XMLNode &operator[](const std::string &name) const;
        std::vector<XMLNode::Ptr> children;

    private:
        XMLNodeType xmlNodeType;
    };
    // ======
    // Prolog
    // ======
    struct XMLNodeProlog : XMLNode
    {
        explicit XMLNodeProlog(XMLNodeType nodeType = XMLNodeType::prolog) : XMLNode(nodeType)
        {
        }
    };
    // ===========
    // Declaration
    // ===========
    struct XMLNodeDeclaration : XMLNode
    {
        explicit XMLNodeDeclaration(XMLNodeType nodeType = XMLNodeType::declaration) : XMLNode(nodeType)
        {
        }
        [[nodiscard]] bool isValidVersion() const
        {
            return (m_version == "1.0" || m_version == "1.1");
        }
        [[nodiscard]] bool isValidEncoding() const
        {
            return (m_encoding == "UTF-8" || m_encoding == "UTF-16");
        }
        [[nodiscard]] bool isValidStandalone() const
        {
            return (m_standalone == "yes" || m_standalone == "no");
        }
        std::string version()
        {
            return (m_version);
        }
        void setVersion(const std::string &version)
        {
            m_version = version;
        }
        std::string encoding()
        {
            return (m_encoding);
        }
        void setEncoding(const std::string &encoding)
        {
            m_encoding = encoding;
        }
        std::string standalone()
        {
            return (m_standalone);
        }
        void setStandalone(const std::string &standalone)
        {
            m_standalone = standalone;
        }

    private:
        std::string m_version{"1.0"};
        std::string m_encoding{"UTF-8"};
        std::string m_standalone{"no"};
    };
    // =======
    // Content
    // =======
    struct XMLNodeContent : XMLNode
    {
        explicit XMLNodeContent(bool isWhiteSpace = true, XMLNodeType nodeType = XMLNodeType::content) : XMLNode(nodeType), m_isWhiteSpace(isWhiteSpace)
        {
        }
        std::string content() const
        {
            return (m_content);
        }
        void addContent(const std::string &content)
        {
            m_content += content;
        }
        bool isWhiteSpace() const
        {
            return (m_isWhiteSpace);
        }
        void setIsWhiteSpace(bool isWhiteSpace)
        {
            m_isWhiteSpace = isWhiteSpace;
        }

    private:
        std::string m_content;
        bool m_isWhiteSpace;
    };
    // =====
    // CDATA
    // =====
    struct XMLNodeCDATA : XMLNode
    {
        explicit XMLNodeCDATA(XMLNodeType nodeType = XMLNodeType::cdata) : XMLNode(nodeType)
        {
        }
        std::string CDATA()
        {
            return (m_cdata);
        }
        void setCDATA(const std::string &cdata)
        {
            m_cdata = cdata;
        }

    private:
        std::string m_cdata;
    };
    // ===============
    // EntityReference
    // ===============
    struct XMLNodeEntityReference : XMLNode
    {
        explicit XMLNodeEntityReference(const XMLValue &value, XMLNodeType nodeType = XMLNodeType::entity) : XMLNode(nodeType), value(std::move(value))
        {
        }
        XMLValue value;
    };
    // =======
    // Element
    // =======
    struct XMLNodeElement : XMLNode
    {
        explicit XMLNodeElement(XMLNodeType nodeType = XMLNodeType::element) : XMLNode(nodeType)
        {
        }
        explicit XMLNodeElement(const std::string &name) : XMLNodeElement()
        {
            this->elementName = name;
        }
        [[nodiscard]] bool isAttributePresent(const std::string &name) const
        {
            return (std::find_if(attributes.rbegin(), attributes.rend(),
                                 [&name](const XMLAttribute &attr)
                                 { return (attr.name == name); }) != attributes.rend());
        }
        void addAttribute(const std::string &name, const XMLValue &value)
        {
            attributes.emplace_back(name, value);
        }
        [[nodiscard]] const XMLAttribute getAttribute(const std::string &name) const
        {
            return (*std::find_if(attributes.rbegin(), attributes.rend(),
                                  [&name](const XMLAttribute &attr)
                                  { return (attr.name == name); }));
        }
        [[nodiscard]] const XMLAttributeList &getAttributeList() const
        {
            return (attributes);
        }
        [[nodiscard]] bool isNameSpacePresent(const std::string &name) const
        {
            return (std::find_if(namespaces.rbegin(), namespaces.rend(),
                                 [&name](const XMLAttribute &attr)
                                 { return (attr.name == name); }) != namespaces.rend());
        }
        void addNameSpace(const std::string &name, const XMLValue &value)
        {
            namespaces.emplace_back(name, value);
        }
        [[nodiscard]] const XMLAttribute getNameSpace(const std::string &name) const
        {
            return (*std::find_if(namespaces.rbegin(), namespaces.rend(),
                                  [&name](const XMLAttribute &ns)
                                  { return (ns.name == name); }));
        }
        [[nodiscard]] const XMLAttributeList &getNameSpaceList() const
        {
            return (namespaces);
        }
        const XMLNodeElement &operator[](int index) const;
        const XMLNodeElement &operator[](const std::string &name) const;
        std::string elementName;

    private:
        XMLAttributeList namespaces;
        XMLAttributeList attributes;
    };
    // =======
    // Comment
    // =======
    struct XMLNodeComment : XMLNode
    {
        explicit XMLNodeComment(XMLNodeType nodeType = XMLNodeType::comment) : XMLNode(nodeType)
        {
        }
        std::string comment;
    };
    // ==
    // PI
    // ==
    struct XMLNodePI : XMLNode
    {
        explicit XMLNodePI(XMLNodeType nodeType = XMLNodeType::pi) : XMLNode(nodeType)
        {
        }
        std::string name;
        std::string parameters;
    };
    // ===
    // DTD
    // ===
    struct XMLNodeDTD : XMLNode
    {
        //
        // DTD Type
        //
        enum Type : uint8_t
        {
            internal = (0x1 << 0),
            external = (0x1 << 1)
        };
        //
        // XML DTD attribute types. Note only one type bit and
        // value bit should be set for each attribute and that
        // some combinations like id and fixed are illegal and
        // are reported as so by the parser.
        //
        enum AttributeType : uint16_t
        {
            // Types bits 0 - 9
            cdata = (0x1 << 0),
            enumeration = (0x1 << 1),
            id = (0x1 << 2),
            idref = (0x1 << 3),
            idrefs = (0x1 << 4),
            nmtoken = (0x1 << 5),
            nmtokens = (0x1 << 6),
            entity = (0x1 << 7),
            entities = (0x1 << 8),
            notation = (0x1 << 9),
            // Value bits 10 - 13
            normal = (0x1 << 10),
            required = (0x1 << 11),
            implied = (0x1 << 12),
            fixed = (0x1 << 13)
        };
        //
        // XML DTD attribute definition
        //
        struct Attribute
        {
            std::string name;
            uint16_t type;
            std::string enumeration;
            XMLValue value{"", ""};
        };
        //
        // XML DTD element definition
        //
        struct Element
        {
            Element() = default;
            Element(const std::string &name, const XMLValue &content) : name(name), content(content) {}
            std::string name;
            bool idAttributePresent = false;
            XMLValue content{"", ""};
            std::vector<Attribute> attributes;
        };
        explicit XMLNodeDTD(IXMLEntityMapper &entityMapper, XMLNodeType nodeType = XMLNodeType::dtd) : XMLNode(nodeType), m_entityMapper(entityMapper)
        {
        }
        const std::string &unparsed() const
        {
            return (m_unparsed);
        }
        void setUnparsed(const std::string &unparsed)
        {
            m_unparsed = unparsed;
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
        XMLNodeDTD::Element &getElement(const std::string &elementName)
        {
            return (m_elements[elementName]);
        }
        void addElement(const std::string &elementName, const XMLNodeDTD::Element &element)
        {
            m_elements.emplace(elementName, element);
        }
        [[nodiscard]] long getElementCount() const
        {
            return (static_cast<long>(m_elements.size()));
        }
        XMLExternalReference &getNotation(const std::string &notationName)
        {
            auto notation = m_notations.find(notationName);
            if (notation != m_notations.end())
            {
                return (notation->second);
            }
            throw XMLNode::Error("Could not find notation name.");
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
        XMLExternalReference m_external{""};
        std::unordered_map<std::string, XMLNodeDTD::Element> m_elements;
        std::unordered_map<std::string, XMLExternalReference> m_notations;
        std::string m_unparsed;
    };
    // ===========================
    // XMLNode reference converter
    // ===========================
    template <typename T>
    void CheckXMLNodeType(auto &xmlNode)
    {
        if constexpr (std::is_same_v<T, XMLNodeProlog>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::prolog)
            {
                throw XMLNode::Error("Node not a prolog.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodeDeclaration>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::declaration)
            {
                throw XMLNode::Error("Node not a declaration.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodeElement>)
        {
            if ((xmlNode.getNodeType() != XMLNodeType::root) &&
                (xmlNode.getNodeType() != XMLNodeType::self) &&
                (xmlNode.getNodeType() != XMLNodeType::element))
            {
                throw XMLNode::Error("Node not a element.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodeContent>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::content)
            {
                throw XMLNode::Error("Node not content.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodeEntityReference>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::entity)
            {
                throw XMLNode::Error("Node not an entity.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodeComment>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::comment)
            {
                throw XMLNode::Error("Node not a comment.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodeCDATA>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::cdata)
            {
                throw XMLNode::Error("Node not CDATA.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodePI>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::pi)
            {
                throw XMLNode::Error("Node not a PI.");
            }
        }
        else if constexpr (std::is_same_v<T, XMLNodeDTD>)
        {
            if (xmlNode.getNodeType() != XMLNodeType::dtd)
            {
                throw XMLNode::Error("Node not DTD.");
            }
        }
    }
    template <typename T>
    T &XMLNodeRef(XMLNode &xmlNode)
    {
        CheckXMLNodeType<T>(xmlNode);
        return (static_cast<T &>(xmlNode));
    }
    template <typename T>
    const T &XMLNodeRef(const XMLNode &xmlNode)
    {
        CheckXMLNodeType<T>(xmlNode);
        return (static_cast<const T &>(xmlNode));
    }
    // ====================
    // XMLNode index access
    // ====================
    inline const XMLNode &XMLNode::operator[](int index) const // Array
    {
        if ((index >= 0) && (index < (static_cast<int>(XMLNodeRef<XMLNode>(*this).children.size()))))
        {
            return (*((XMLNodeRef<XMLNode>(*this).children[index])));
        }
        throw XMLNode::Error("Invalid index used to access array.");
    }
    // ===================
    // XMLNode name access
    // ===================
    inline const XMLNode &XMLNode::operator[](const std::string &name) const // Array
    {
        if (xmlNodeType <= XMLNodeType::element)
        {
            for (const auto &element : XMLNodeRef<XMLNodeElement>(*this).children)
            {
                if (XMLNodeRef<XMLNodeElement>(*element).elementName == name)
                {
                    return (*element);
                }
            }
        }
        throw XMLNode::Error("Invalid index used to access array.");
    }
    // ===========================
    // XMLNodeElement index access
    // ===========================
    inline const XMLNodeElement &XMLNodeElement::operator[](int index) const // Array
    {
        int number = 0;
        if ((index >= 0) && (index < (static_cast<int>(XMLNodeRef<XMLNodeElement>(*this).children.size()))))
        {
            for (const auto &child : XMLNodeRef<XMLNode>(*this).children)
            {
                if (XMLNodeRef<XMLNode>(*child).getNodeType() <= XMLNodeType::element)
                {
                    if (number == index)
                    {
                        return (XMLNodeRef<XMLNodeElement>(*child));
                    }
                    number++;
                }
            }
        }
        throw XMLNode::Error("Invalid index used to access array.");
    }
    // ==========================
    // XMLNodeElement name access
    // ==========================
    inline const XMLNodeElement &XMLNodeElement::operator[](const std::string &name) const // Array
    {
        if (getNodeType() <= XMLNodeType::element)
        {
            for (const auto &element : XMLNodeRef<XMLNodeElement>(*this).children)
            {
                if (XMLNodeRef<XMLNodeElement>(*element).elementName == name)
                {
                    return (XMLNodeRef<XMLNodeElement>(*element));
                }
            }
        }
        throw XMLNode::Error("Invalid index used to access array.");
    }
    // ====================
    // XMLNode get contents
    // ====================
    inline const std::string XMLNode::getContents() const
    {
        std::string result;
        for (const auto &node : children)
        {
            if (node->getNodeType() == XMLNodeType::content)
            {
                result += XMLNodeRef<XMLNodeContent>(*node).content();
            }
            else if (node->getNodeType() == XMLNodeType::entity)
            {
                if (!XMLNodeRef<XMLNodeEntityReference>(*node).children.empty())
                {
                    result += XMLNodeRef<XMLNodeEntityReference>(*node).getContents();
                }
                else
                {
                    result += XMLNodeRef<XMLNodeEntityReference>(*node).value.parsed;
                }
            }
            else if (node->getNodeType() == XMLNodeType::cdata)
            {
                result += XMLNodeRef<XMLNodeCDATA>(*node).CDATA();
            }
        }
        return (result);
    }
} // namespace XMLLib
