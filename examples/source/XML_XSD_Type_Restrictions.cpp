// Program: XML_XSD_Type_Restrictions
//
// Description: Demonstrates XSD simple type restriction facets using XML_Lib.
// Covers minInclusive/maxInclusive, pattern, enumeration, and minLength/maxLength
// on named simpleType definitions.
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

// ---- Schemas ---------------------------------------------------------------

// Integer range: age must be between 0 and 150 inclusive
static const std::string kAgeSchema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:simpleType name="AgeType">
    <xs:restriction base="xs:integer">
      <xs:minInclusive value="0"/>
      <xs:maxInclusive value="150"/>
    </xs:restriction>
  </xs:simpleType>
  <xs:element name="age" type="AgeType"/>
</xs:schema>
)";

// Pattern: UK postcode format
static const std::string kPostcodeSchema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:simpleType name="UKPostcode">
    <xs:restriction base="xs:string">
      <xs:pattern value="[A-Z]{1,2}[0-9][0-9A-Z]?\s[0-9][A-Z]{2}"/>
    </xs:restriction>
  </xs:simpleType>
  <xs:element name="postcode" type="UKPostcode"/>
</xs:schema>
)";

// Enumeration: status must be one of three values
static const std::string kStatusSchema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:simpleType name="StatusType">
    <xs:restriction base="xs:string">
      <xs:enumeration value="active"/>
      <xs:enumeration value="inactive"/>
      <xs:enumeration value="pending"/>
    </xs:restriction>
  </xs:simpleType>
  <xs:element name="status" type="StatusType"/>
</xs:schema>
)";

// String length: username between 3 and 20 characters
static const std::string kUsernameSchema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:simpleType name="UsernameType">
    <xs:restriction base="xs:string">
      <xs:minLength value="3"/>
      <xs:maxLength value="20"/>
    </xs:restriction>
  </xs:simpleType>
  <xs:element name="username" type="UsernameType"/>
</xs:schema>
)";

// ---- Main ------------------------------------------------------------------

int main()
{
  init(plog::debug, "XML_XSD_Type_Restrictions.log");
  PLOG_INFO << "XML_XSD_Type_Restrictions started ...";

  // --- Integer range ---
  tryValidate("Age 25 (valid)", R"(<age>25</age>)", kAgeSchema);
  tryValidate("Age 200 (too high)", R"(<age>200</age>)", kAgeSchema);
  tryValidate("Age -1 (too low)", R"(<age>-1</age>)", kAgeSchema);
  tryValidate("Age 150 (boundary)", R"(<age>150</age>)", kAgeSchema);

  // --- Pattern ---
  tryValidate("Postcode 'SW1A 1AA' (valid)", R"(<postcode>SW1A 1AA</postcode>)", kPostcodeSchema);
  tryValidate("Postcode 'invalid' (bad)", R"(<postcode>invalid</postcode>)", kPostcodeSchema);

  // --- Enumeration ---
  tryValidate("Status 'active' (valid)", R"(<status>active</status>)", kStatusSchema);
  tryValidate("Status 'deleted' (unknown)", R"(<status>deleted</status>)", kStatusSchema);

  // --- String length ---
  tryValidate("Username 'jo' (too short)", R"(<username>jo</username>)", kUsernameSchema);
  tryValidate("Username 'alice' (valid)", R"(<username>alice</username>)", kUsernameSchema);
  tryValidate("Username (21 chars, too long)", R"(<username>averylongusernameXX</username>)", kUsernameSchema);

  PLOG_INFO << "XML_XSD_Type_Restrictions exited.";
  return EXIT_SUCCESS;
}
