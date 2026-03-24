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
- **Parsing from String**: Use `XMLParser` to parse XML from strings.
- **Custom Attributes**: Add and manipulate attributes using `XMLAttribute` methods.

## Examples
See the [examples](../examples/) directory for more sample code.

---
*For API details, see the [API Reference](API.md).*