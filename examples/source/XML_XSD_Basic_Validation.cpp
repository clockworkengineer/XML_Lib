// Program: XML_XSD_Basic_Validation
//
// Description: Demonstrates basic XSD schema validation using XML_Lib.
// Shows how to validate a well-formed XML document against an inline XSD
// schema, and how to catch and report schema violations.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

// ---- Schema ----------------------------------------------------------------

static const std::string kNoteSchema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">

  <xs:element name="note">
    <xs:complexType>
      <xs:sequence>
        <xs:element name="to"      type="xs:string"/>
        <xs:element name="from"    type="xs:string"/>
        <xs:element name="heading" type="xs:string"/>
        <xs:element name="body"    type="xs:string"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

</xs:schema>
)";

// ---- Helpers ---------------------------------------------------------------

static void tryValidate(const std::string &label, const std::string &xmlSource, const std::string &schema)
{
  std::cout << "--- " << label << " ---" << std::endl;
  try {
    xl::XML xml;
    xl::BufferSource source{ xmlSource };
    xml.parse(source);
    xml.validate(schema);
    std::cout << "Validation passed." << std::endl;
  } catch (const xl::IValidator::Error &e) {
    std::cerr << "Validation error: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
  }
}

// ---- Main ------------------------------------------------------------------

/// <summary>Program entry point.</summary>
int main()
{
  std::cout << "XML_XSD_Basic_Validation started ..." << std::endl;
  // 1. Valid document — all four required child elements present
  tryValidate("Valid note document",
    R"(<?xml version="1.0"?>
    <note>
      <to>Alice</to>
      <from>Bob</from>
      <heading>Reminder</heading>
      <body>Don't forget the meeting!</body>
    </note>)",
    kNoteSchema);

  // 2. Missing required element — 'body' is absent
  tryValidate("Missing 'body' element",
    R"(<?xml version="1.0"?>
    <note>
      <to>Alice</to>
      <from>Bob</from>
      <heading>Reminder</heading>
    </note>)",
    kNoteSchema);

  // 3. Extra unexpected element — 'signature' is not in the schema
  tryValidate("Unexpected child element 'signature'",
    R"(<?xml version="1.0"?>
    <note>
      <to>Alice</to>
      <from>Bob</from>
      <heading>Reminder</heading>
      <body>Hello!</body>
      <signature>Bob</signature>
    </note>)",
    kNoteSchema);

  // 4. Wrong root element — validator expects 'note', gets 'message'
  tryValidate("Wrong root element",
    R"(<?xml version="1.0"?>
    <message>
      <to>Alice</to>
    </message>)",
    kNoteSchema);

  std::cout << "XML_XSD_Basic_Validation exited." << std::endl;
  return EXIT_SUCCESS;
}
