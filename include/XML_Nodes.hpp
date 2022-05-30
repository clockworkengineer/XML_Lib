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
    //
    // Pointer to XMLNode
    //
    struct XMLNode;
    using XMLNodePtr = std::unique_ptr<XMLNode>;
    //
    // XML Node structure
    //
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
    //
    // Base XMLNode
    //
    struct XMLNode
    {
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
        std::vector<XMLNodePtr> children;

    private:
        XMLNodeType xmlNodeType;
    };
    //
    // Prolog XMLNode
    //
    struct XMLNodeProlog : XMLNode
    {
        explicit XMLNodeProlog(XMLNodeType nodeType = XMLNodeType::prolog) : XMLNode(nodeType)
        {
        }
    };
    //
    // Declaration XMLNode
    //
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
    //
    // Content XMLNode
    //
    struct XMLNodeContent : XMLNode
    {
        explicit XMLNodeContent(XMLNodeType nodeType = XMLNodeType::content) : XMLNode(nodeType)
        {
        }
        std::string content;
        bool isWhiteSpace = true;
    };
    //
    // CDATA XMLNode
    //
    struct XMLNodeCDATA : XMLNode
    {
        explicit XMLNodeCDATA(XMLNodeType nodeType = XMLNodeType::cdata) : XMLNode(nodeType)
        {
        }
        std::string cdata;
    };
    //
    // EntityReference XMLNode
    //
    struct XMLNodeEntityReference : XMLNode
    {
        explicit XMLNodeEntityReference(const XMLValue &value, XMLNodeType nodeType = XMLNodeType::entity) : XMLNode(nodeType), value(std::move(value))
        {
        }
        XMLValue value;
    };
    //
    // Element XMLNode
    //
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
    //
    // Comment XMLNode
    //
    struct XMLNodeComment : XMLNode
    {
        explicit XMLNodeComment(XMLNodeType nodeType = XMLNodeType::comment) : XMLNode(nodeType)
        {
        }
        std::string comment;
    };
    //
    // PI XMLNode
    //
    struct XMLNodePI : XMLNode
    {
        explicit XMLNodePI(XMLNodeType nodeType = XMLNodeType::pi) : XMLNode(nodeType)
        {
        }
        std::string name;
        std::string parameters;
    };
    //
    // DTD XMLNode
    //
    struct XMLNodeDTD : XMLNode
    {
        explicit XMLNodeDTD(XMLNodeType nodeType = XMLNodeType::dtd) : XMLNode(nodeType)
        {
        }
    };
    //
    // Convert base XMLNode reference
    //
    template <typename T>
    T &XMLNodeRef(XMLNode &xmlNode)
    {
        return (static_cast<T &>(xmlNode));
    }
    template <typename T>
    const T &XMLNodeRef(const XMLNode &xmlNode)
    {
        return (static_cast<const T &>(xmlNode));
    }
    //
    // XMLNode index access
    //
    inline const XMLNode &XMLNode::operator[](int index) const // Array
    {
        if ((index >= 0) && (index < (static_cast<int>(XMLNodeRef<XMLNode>(*this).children.size()))))
        {
            return (*((XMLNodeRef<XMLNode>(*this).children[index])));
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XMLNode name access
    //
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
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XNodeElement index access
    //
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
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XNodeElement name access
    //
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
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XMLNode get contents
    //
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
