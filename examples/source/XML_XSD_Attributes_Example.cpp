// Program: XML_XSD_Attributes_Example
//
// Description: Demonstrates XSD attribute validation using XML_Lib.
// Shows required, optional, prohibited, fixed-value, and anyAttribute
// wildcard declarations, plus a schema-file-based validation workflow.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

// ---- Helpers ---------------------------------------------------------------

static void tryValidate(const std::string &label, const std::string &xmlSource, const std::string &schema)
{
  PLOG_INFO << "--- " << label << " ---";
  try {
    xl::XML xml;
    xl::BufferSource source{ xmlSource };
    xml.parse(source);
    xml.validate(schema);
    PLOG_INFO << "Validation passed.";
  } catch (const xl::IValidator::Error &e) {
    PLOG_WARNING << "Validation error: " << e.what();
  } catch (const std::exception &e) {
    PLOG_ERROR << "Unexpected error: " << e.what();
  }
}

// ---- Schema ----------------------------------------------------------------

// person element:
//   - id       required integer
//   - version  fixed "1.0" (must equal "1.0" when present)
//   - email    optional string
//   - legacy   prohibited (must not appear)
static const std::string kPersonSchema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="person">
    <xs:complexType>
      <xs:sequence>
        <xs:element name="name" type="xs:string"/>
      </xs:sequence>
      <xs:attribute name="id"      type="xs:integer" use="required"/>
      <xs:attribute name="version" type="xs:string"  use="optional" fixed="1.0"/>
      <xs:attribute name="email"   type="xs:string"  use="optional"/>
      <xs:attribute name="legacy"  type="xs:string"  use="prohibited"/>
    </xs:complexType>
  </xs:element>
</xs:schema>
)";

// product element: declares xs:anyAttribute so any extra attribute is accepted
static const std::string kProductSchema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="product">
    <xs:complexType>
      <xs:sequence>
        <xs:element name="title" type="xs:string"/>
      </xs:sequence>
      <xs:attribute name="sku" type="xs:string" use="required"/>
      <xs:anyAttribute/>
    </xs:complexType>
  </xs:element>
</xs:schema>
)";

// ---- Main ------------------------------------------------------------------

int main()
{
  init(plog::debug, "XML_XSD_Attributes_Example.log");
  PLOG_INFO << "XML_XSD_Attributes_Example started ...";

  // --- Required attribute ---
  tryValidate("Required 'id' present (valid)", R"(<person id="42"><name>Alice</name></person>)", kPersonSchema);

  tryValidate("Required 'id' missing (error)", R"(<person><name>Alice</name></person>)", kPersonSchema);

  // --- Fixed attribute value ---
  tryValidate("Fixed 'version' correct value (valid)",
    R"(<person id="1" version="1.0"><name>Bob</name></person>)",
    kPersonSchema);

  tryValidate(
    "Fixed 'version' wrong value (error)", R"(<person id="1" version="2.0"><name>Bob</name></person>)", kPersonSchema);

  // --- Prohibited attribute ---
  tryValidate(
    "Prohibited 'legacy' present (error)", R"(<person id="1" legacy="yes"><name>Carol</name></person>)", kPersonSchema);

  // --- Undeclared attribute ---
  tryValidate("Undeclared 'nickname' attribute (error)",
    R"(<person id="1" nickname="dave"><name>Dave</name></person>)",
    kPersonSchema);

  // --- anyAttribute wildcard ---
  tryValidate("Extra attributes with anyAttribute (valid)",
    R"(<product sku="ABC-001" color="red" weight="0.5kg"><title>Widget</title></product>)",
    kProductSchema);

  PLOG_INFO << "XML_XSD_Attributes_Example exited.";
  return EXIT_SUCCESS;
}
