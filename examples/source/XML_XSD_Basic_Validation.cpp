// Program: XML_XSD_Basic_Validation
//
// Description: Demonstrates basic XSD schema validation using XML_Lib.
// Shows how to validate a well-formed XML document against an inline XSD
// schema, and how to catch and report schema violations.
//
// Dependencies: C++20, PLOG, XML_Lib.
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

// ---- Main ------------------------------------------------------------------

int main()
{
  init(plog::debug, "XML_XSD_Basic_Validation.log");
  PLOG_INFO << "XML_XSD_Basic_Validation started ...";

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

  PLOG_INFO << "XML_XSD_Basic_Validation exited.";
  return EXIT_SUCCESS;
}
