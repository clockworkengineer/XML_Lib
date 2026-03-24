# XML_Lib API Reference

This document provides a comprehensive reference for the public API of the XML_Lib library.

## Overview
XML_Lib is a C++ library for parsing, creating, and manipulating XML files. It is designed for performance, flexibility, and ease of integration.

## Namespaces
- `xml_lib`: Main namespace for all library components.

## Main Classes
- `XMLDocument`: Represents an XML document. Provides methods for loading, saving, and manipulating XML data.
- `XMLElement`: Represents an element node in the XML tree.
- `XMLAttribute`: Represents an attribute of an XML element.
- `XMLParser`: Handles parsing of XML files and strings.

## Key Methods
### XMLDocument
- `bool LoadFile(const std::string& filename);`
- `bool SaveFile(const std::string& filename) const;`
- `XMLElement* RootElement();`

### XMLElement
- `std::string Name() const;`
- `std::string GetText() const;`
- `XMLElement* FirstChildElement(const std::string& name = "");`
- `XMLAttribute* FindAttribute(const std::string& name);`

### XMLAttribute
- `std::string Name() const;`
- `std::string Value() const;`

## Error Handling
- Most methods return `bool` for success/failure or throw exceptions on error.

## Example Usage
See the [Guide](Guide.md) and [Examples](../examples/) for practical usage scenarios.

---
*For detailed class and method documentation, refer to the source code in the `include/` and `source/` directories.*
