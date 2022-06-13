#pragma once
//
// C++ STL
//
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
// =========
// NAMESPACE
// =========
namespace XMLLib
{ 
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    class IXMLEntityMapper;
    struct DTDParsed;
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
            return (version == "1.0" || version == "1.1");
        }
        [[nodiscard]] bool isValidEncoding() const
        {
            return (encoding == "UTF-8" || encoding == "UTF-16");
        }
        [[nodiscard]] bool isValidStandalone() const
        {
            return (standalone == "yes" || standalone == "no");
        }
        std::string version{"1.0"};
        std::string encoding{"UTF-8"};
        std::string standalone{"no"};
    };
    // =======
    // Content
    // =======
    struct XMLNodeContent : XMLNode
    {
        explicit XMLNodeContent(bool isWhiteSpace = true, XMLNodeType nodeType = XMLNodeType::content) : XMLNode(nodeType), isWhiteSpace(isWhiteSpace)
        {
        }
        std::string content;
        bool isWhiteSpace;
    };
    // =====
    // CDATA
    // =====
    struct XMLNodeCDATA : XMLNode
    {
        explicit XMLNodeCDATA(XMLNodeType nodeType = XMLNodeType::cdata) : XMLNode(nodeType)
        {
        }
        std::string cdata;
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
        explicit XMLNodeDTD(IXMLEntityMapper &entityMapper, XMLNodeType nodeType = XMLNodeType::dtd) : XMLNode(nodeType)
        {
             m_parsed = std::make_unique<DTDParsed>(entityMapper);
        }
        std::unique_ptr<DTDParsed> m_parsed;
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
                result += XMLNodeRef<XMLNodeContent>(*node).content;
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
                result += XMLNodeRef<XMLNodeCDATA>(*node).cdata;
            }
        }
        return (result);
    }
} // namespace XMLLib
