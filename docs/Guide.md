# XML_Lib User Guide

This guide provides step-by-step instructions for using the XML_Lib library in your C++ projects.

## Getting Started
1. **Include the Library**: Add the `include/` directory to your project's include path.
2. **Link the Library**: Link against the compiled XML_Lib library.
3. **Add Required Headers**:
   ```cpp
   #include <XMLDocument.hpp>
   #include <XMLElement.hpp>
   #include <XMLAttribute.hpp>
   ```

## Basic Usage
### Loading an XML File
```cpp
XMLDocument doc;
bool loaded = doc.LoadFile("example.xml");
if (!loaded) {
    // Handle error
}
```

### Accessing Elements and Attributes
```cpp
XMLElement* root = doc.RootElement();
if (root) {
    std::string name = root->Name();
    XMLElement* child = root->FirstChildElement("item");
    if (child) {
        XMLAttribute* attr = child->FindAttribute("id");
        if (attr) {
            std::string value = attr->Value();
        }
    }
}
```

### Creating and Saving XML
```cpp
XMLDocument doc;
XMLElement* root = doc.NewElement("root");
doc.InsertFirstChild(root);
XMLElement* child = doc.NewElement("item");
child->SetText("Hello, XML!");
root->InsertEndChild(child);
doc.SaveFile("output.xml");
```

## Advanced Topics
- **Error Handling**: Check return values or catch exceptions for robust error management.
- **Parsing from String**: Use `BufferSource` to parse XML from strings.
- **Custom Attributes**: Add and manipulate attributes using `addAttribute` / `getAttributes`.
- **XML Namespaces**: See the Namespace section below.

## XML Namespaces

XML_Lib fully supports the [W3C XML Namespaces](https://www.w3.org/TR/xml-names/) recommendation.

### Default Namespace
```cpp
XML xml;
BufferSource source{
  "<table xmlns=\"http://www.w3.org/TR/html4/\">"
  "<tr><td>Data</td></tr>"
  "</table>"
};
xml.parse(source);
auto &root = NRef<Element>(xml.root());
// Default namespace stored under key ":"
std::cout << root[0].getNameSpace(":").getParsed();  // "http://www.w3.org/TR/html4/"
std::cout << root[0].getNamespaceURI();              // "http://www.w3.org/TR/html4/"
```

### Prefixed Namespaces
```cpp
XML xml;
BufferSource source{
  "<root xmlns:h=\"http://www.w3.org/TR/html4/\">"
  "<h:table><h:tr><h:td>Data</h:td></h:tr></h:table>"
  "</root>"
};
xml.parse(source);
auto &root = NRef<Element>(xml.root());
auto &table = root[0];
std::cout << table.name();             // "h:table"
std::cout << table.getPrefix();        // "h"
std::cout << table.getLocalName();     // "table"
std::cout << table.getNamespaceURI();  // "http://www.w3.org/TR/html4/"
```

### Namespace Scoping
Namespaces declared on a parent element are inherited by all children. `getNameSpaces()` returns all in-scope declarations accumulated from the root down to that element:
```cpp
// root declares xmlns:h and xmlns:f; both are visible on child elements
auto &child = root[0];
child.getNameSpaces();  // contains both "h" and "f" mappings
```

### Error Cases
- Using an undeclared prefix throws `SyntaxError`: _"Namespace used but not defined."_
- Declaring the same prefix twice on one element throws `SyntaxError`: _"Attribute 'xmlns:f' defined more than once within start tag."_
- Using a prefixed attribute where the prefix is undeclared throws `SyntaxError`: _"Namespace used but not defined in attribute 'x:border'."_

## Examples
See the [examples](../examples/) directory for more sample code.

---
*For API details, see the [API Reference](API.md).*