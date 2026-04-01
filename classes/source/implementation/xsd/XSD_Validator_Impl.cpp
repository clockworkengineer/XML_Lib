//
// Class: XSD_Impl
//
// Description: XSD_Validator class constructor / destructor and top-level dispatch.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// XSD_Impl constructor.
/// </summary>
XSD_Impl::XSD_Impl(Node &xNode) : xmlRoot(xNode) {}

/// <summary>
/// XSD_Impl destructor.
/// </summary>
XSD_Impl::~XSD_Impl() = default;

/// <summary>
/// Parse XSD schema from source stream.
/// </summary>
/// <param name="source">XSD source stream.</param>
void XSD_Impl::parse(ISource &source)
{
  // Parse the XSD file as XML using the library itself
  XML xsdXml;
  xsdXml.parse(source);
  // Walk the resulting Node tree to populate the schema data model
  parseSchema(xsdXml.root());
}

/// <summary>
/// Stringify the loaded schema back to XML text.
/// </summary>
/// <param name="destination">Destination stream.</param>
void XSD_Impl::stringify(IDestination &destination) const
{
  destination.add("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  destination.add("<xs:schema");
  if (!targetNamespace.empty()) { destination.add(" targetNamespace=\"" + targetNamespace + "\""); }
  destination.add(" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">");
  destination.add("\n");

  for (const auto &[name, st] : simpleTypes) {
    destination.add("  <xs:simpleType name=\"" + name + "\">\n");
    destination.add("    <xs:restriction base=\"" + st.baseType + "\"/>\n");
    destination.add("  </xs:simpleType>\n");
  }

  for (const auto &elemDecl : rootElements) {
    destination.add("  <xs:element name=\"" + elemDecl.name + "\"");
    if (!elemDecl.typeRef.empty()) { destination.add(" type=\"" + elemDecl.typeRef + "\""); }
    destination.add("/>\n");
  }

  destination.add("</xs:schema>\n");
}

}// namespace XML_Lib
