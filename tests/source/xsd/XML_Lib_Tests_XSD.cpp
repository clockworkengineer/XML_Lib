#include "XML_Lib_Tests.hpp"

/// <summary>
/// Validate xmlSource against the given XSD schema string.
/// Returns empty string on success, exception message on failure.
/// </summary>
static std::string validateXSD(const std::string &xmlSource, const std::string &xsdSchema)
{
  try {
    XML xml(xmlSource);
    xml.validate(xsdSchema);
    return "";
  } catch (const std::exception &e) {
    return e.what();
  }
}

/// <summary>
/// Validate xmlSource against the given XSD schema file path.
/// Returns empty string on success, exception message on failure.
/// </summary>
static std::string validateXSDFile(const std::string &xmlSource, const std::string &xsdFilePath)
{
  return validateXSD(xmlSource, XML::fromFile(xsdFilePath));
}

TEST_CASE("Parse and load XSD schemas.", "[XML][XSD][Parse]")
{
  SECTION("Parse a simple xs:string element schema.", "[XML][XSD][Parse]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"note\" type=\"xs:string\"/>\n"
      "</xs:schema>\n"
    };
    XML xml{ "<note>Hello</note>" };
    REQUIRE_NOTHROW(xml.validate(xsd));
  }
  SECTION("Parse a named complexType schema.", "[XML][XSD][Parse]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"note\" type=\"NoteType\"/>\n"
      "  <xs:complexType name=\"NoteType\">\n"
      "    <xs:sequence>\n"
      "      <xs:element name=\"to\" type=\"xs:string\"/>\n"
      "      <xs:element name=\"from\" type=\"xs:string\"/>\n"
      "    </xs:sequence>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    XML xml{ "<note><to>Alice</to><from>Bob</from></note>" };
    REQUIRE_NOTHROW(xml.validate(xsd));
  }
  SECTION("Parse a schema with a named simpleType restriction.", "[XML][XSD][Parse]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"score\" type=\"ScoreType\"/>\n"
      "  <xs:simpleType name=\"ScoreType\">\n"
      "    <xs:restriction base=\"xs:integer\">\n"
      "      <xs:minInclusive value=\"0\"/>\n"
      "      <xs:maxInclusive value=\"100\"/>\n"
      "    </xs:restriction>\n"
      "  </xs:simpleType>\n"
      "</xs:schema>\n"
    };
    XML xml{ "<score>75</score>" };
    REQUIRE_NOTHROW(xml.validate(xsd));
  }
  SECTION("Schema with non-xs:schema root element throws.", "[XML][XSD][Parse]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<root xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"/>\n"
    };
    XML xml{ "<note>Hello</note>" };
    REQUIRE_THROWS_WITH(xml.validate(xsd), "IValidator Error: XSD root element must be 'xs:schema', found: 'root'.");
  }
  SECTION("Parse schema from file — simple string element.", "[XML][XSD][Parse]")
  {
    XML xml{ "<note>Hello World</note>" };
    REQUIRE_NOTHROW(xml.validate(XML::fromFile(prefixTestDataPath("xsd/simple_string.xsd"))));
  }
  SECTION("Parse schema from file — complex sequence.", "[XML][XSD][Parse]")
  {
    XML xml{ "<note><to>Alice</to><from>Bob</from><heading>Reminder</heading><body>Call me</body></note>" };
    REQUIRE_NOTHROW(xml.validate(XML::fromFile(prefixTestDataPath("xsd/complex_sequence.xsd"))));
  }
}

TEST_CASE("XSD element validation.", "[XML][XSD][Validate][Elements]")
{
  SECTION("Valid XML passes sequence content model.", "[XML][XSD][Validate][Elements]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"note\" type=\"NoteType\"/>\n"
      "  <xs:complexType name=\"NoteType\">\n"
      "    <xs:sequence>\n"
      "      <xs:element name=\"to\" type=\"xs:string\"/>\n"
      "      <xs:element name=\"from\" type=\"xs:string\"/>\n"
      "    </xs:sequence>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<note><to>Alice</to><from>Bob</from></note>", xsd).empty());
  }
  SECTION("Missing required child element fails.", "[XML][XSD][Validate][Elements]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"note\" type=\"NoteType\"/>\n"
      "  <xs:complexType name=\"NoteType\">\n"
      "    <xs:sequence>\n"
      "      <xs:element name=\"to\" type=\"xs:string\"/>\n"
      "      <xs:element name=\"from\" type=\"xs:string\"/>\n"
      "    </xs:sequence>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    const auto result = validateXSD("<note><to>Alice</to></note>", xsd);
    REQUIRE(result.find("from") != std::string::npos);
    REQUIRE(result.find("missing") != std::string::npos);
  }
  SECTION("Unexpected child element fails.", "[XML][XSD][Validate][Elements]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"note\" type=\"NoteType\"/>\n"
      "  <xs:complexType name=\"NoteType\">\n"
      "    <xs:sequence>\n"
      "      <xs:element name=\"to\" type=\"xs:string\"/>\n"
      "    </xs:sequence>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    const auto result = validateXSD("<note><to>Alice</to><extra>oops</extra></note>", xsd);
    REQUIRE(result.find("unexpected") != std::string::npos);
    REQUIRE(result.find("extra") != std::string::npos);
  }
  SECTION("Root element not in schema fails.", "[XML][XSD][Validate][Elements]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"note\" type=\"xs:string\"/>\n"
      "</xs:schema>\n"
    };
    const auto result = validateXSD("<other>Hello</other>", xsd);
    REQUIRE(result.find("not declared") != std::string::npos);
  }
  SECTION("maxOccurs exceeded fails.", "[XML][XSD][Validate][Elements]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"list\" type=\"ListType\"/>\n"
      "  <xs:complexType name=\"ListType\">\n"
      "    <xs:sequence>\n"
      "      <xs:element name=\"item\" type=\"xs:string\" maxOccurs=\"2\"/>\n"
      "    </xs:sequence>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    const auto result = validateXSD("<list><item>a</item><item>b</item><item>c</item></list>", xsd);
    REQUIRE(result.find("maxOccurs") != std::string::npos);
  }
  SECTION("maxOccurs unbounded allows many children.", "[XML][XSD][Validate][Elements]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"list\" type=\"ListType\"/>\n"
      "  <xs:complexType name=\"ListType\">\n"
      "    <xs:sequence>\n"
      "      <xs:element name=\"item\" type=\"xs:string\" minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n"
      "    </xs:sequence>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<list><item>a</item><item>b</item><item>c</item><item>d</item></list>", xsd).empty());
  }
  SECTION("xs:choice with single valid branch passes.", "[XML][XSD][Validate][Elements]")
  {
    REQUIRE(
      validateXSDFile("<payment><creditCard>4111</creditCard></payment>", prefixTestDataPath("xsd/complex_choice.xsd"))
        .empty());
  }
  SECTION("xs:choice with two branches present fails.", "[XML][XSD][Validate][Elements]")
  {
    const auto result =
      validateXSDFile("<payment><creditCard>4111</creditCard><paypal>user@example.com</paypal></payment>",
        prefixTestDataPath("xsd/complex_choice.xsd"));
    REQUIRE(result.find("choice") != std::string::npos);
    REQUIRE(result.find("one") != std::string::npos);
  }
  SECTION("xs:choice with no branch present fails.", "[XML][XSD][Validate][Elements]")
  {
    const auto result = validateXSDFile("<payment/>", prefixTestDataPath("xsd/complex_choice.xsd"));
    REQUIRE(result.find("choice") != std::string::npos);
  }
}

TEST_CASE("XSD attribute validation.", "[XML][XSD][Validate][Attributes]")
{
  SECTION("Required attribute present passes.", "[XML][XSD][Validate][Attributes]")
  {
    REQUIRE(
      validateXSDFile("<person id=\"1\"><name>Alice</name></person>", prefixTestDataPath("xsd/complex_attributes.xsd"))
        .empty());
  }
  SECTION("Missing required attribute fails.", "[XML][XSD][Validate][Attributes]")
  {
    const auto result =
      validateXSDFile("<person><name>Alice</name></person>", prefixTestDataPath("xsd/complex_attributes.xsd"));
    REQUIRE(result.find("id") != std::string::npos);
    REQUIRE(result.find("required") != std::string::npos);
  }
  SECTION("Prohibited attribute present fails.", "[XML][XSD][Validate][Attributes]")
  {
    const auto result = validateXSDFile(
      "<person id=\"1\" locked=\"true\"><name>Alice</name></person>", prefixTestDataPath("xsd/complex_attributes.xsd"));
    REQUIRE(result.find("locked") != std::string::npos);
    REQUIRE(result.find("prohibited") != std::string::npos);
  }
  SECTION("Undeclared attribute fails.", "[XML][XSD][Validate][Attributes]")
  {
    const auto result = validateXSDFile(
      "<person id=\"1\" unknown=\"x\"><name>Alice</name></person>", prefixTestDataPath("xsd/complex_attributes.xsd"));
    REQUIRE(result.find("unknown") != std::string::npos);
    REQUIRE(result.find("undeclared") != std::string::npos);
  }
  SECTION("Optional attribute absent passes.", "[XML][XSD][Validate][Attributes]")
  {
    REQUIRE(
      validateXSDFile("<person id=\"1\"><name>Alice</name></person>", prefixTestDataPath("xsd/complex_attributes.xsd"))
        .empty());
  }
  SECTION("Fixed attribute value must match.", "[XML][XSD][Validate][Attributes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"widget\" type=\"WidgetType\"/>\n"
      "  <xs:complexType name=\"WidgetType\">\n"
      "    <xs:sequence/>\n"
      "    <xs:attribute name=\"version\" type=\"xs:string\" fixed=\"2.0\"/>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    const auto result = validateXSD("<widget version=\"1.0\"/>", xsd);
    REQUIRE(result.find("fixed") != std::string::npos);
    REQUIRE(result.find("2.0") != std::string::npos);
  }
  SECTION("xs:anyAttribute allows unknown attributes.", "[XML][XSD][Validate][Attributes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"widget\" type=\"WidgetType\"/>\n"
      "  <xs:complexType name=\"WidgetType\">\n"
      "    <xs:sequence/>\n"
      "    <xs:anyAttribute/>\n"
      "  </xs:complexType>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<widget foo=\"bar\" baz=\"qux\"/>", xsd).empty());
  }
}

TEST_CASE("XSD simple type validation.", "[XML][XSD][Validate][SimpleTypes]")
{
  SECTION("xs:boolean accepts 'true'.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"flag\" type=\"xs:boolean\"/>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<flag>true</flag>", xsd).empty());
    REQUIRE(validateXSD("<flag>false</flag>", xsd).empty());
    REQUIRE(validateXSD("<flag>1</flag>", xsd).empty());
    REQUIRE(validateXSD("<flag>0</flag>", xsd).empty());
  }
  SECTION("xs:boolean rejects non-boolean value.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"flag\" type=\"xs:boolean\"/>\n"
      "</xs:schema>\n"
    };
    const auto result = validateXSD("<flag>yes</flag>", xsd);
    REQUIRE(result.find("xs:boolean") != std::string::npos);
  }
  SECTION("xs:integer accepts valid integer.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"count\" type=\"xs:integer\"/>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<count>42</count>", xsd).empty());
    REQUIRE(validateXSD("<count>-7</count>", xsd).empty());
  }
  SECTION("xs:integer rejects non-integer value.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"count\" type=\"xs:integer\"/>\n"
      "</xs:schema>\n"
    };
    const auto result = validateXSD("<count>3.14</count>", xsd);
    REQUIRE(result.find("xs:integer") != std::string::npos);
  }
  SECTION("minInclusive restriction enforced.", "[XML][XSD][Validate][SimpleTypes]")
  {
    REQUIRE(validateXSDFile("<age>25</age>", prefixTestDataPath("xsd/simple_integer.xsd")).empty());
    const auto result = validateXSDFile("<age>-1</age>", prefixTestDataPath("xsd/simple_integer.xsd"));
    REQUIRE(result.find("minInclusive") != std::string::npos);
  }
  SECTION("maxInclusive restriction enforced.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const auto result = validateXSDFile("<age>200</age>", prefixTestDataPath("xsd/simple_integer.xsd"));
    REQUIRE(result.find("maxInclusive") != std::string::npos);
  }
  SECTION("pattern restriction enforced.", "[XML][XSD][Validate][SimpleTypes]")
  {
    REQUIRE(validateXSDFile("<postcode>SW1A 1AA</postcode>", prefixTestDataPath("xsd/simple_pattern.xsd")).empty());
    const auto result = validateXSDFile("<postcode>12345</postcode>", prefixTestDataPath("xsd/simple_pattern.xsd"));
    REQUIRE(result.find("pattern") != std::string::npos);
  }
  SECTION("enumeration restriction enforced.", "[XML][XSD][Validate][SimpleTypes]")
  {
    REQUIRE(validateXSDFile("<status>active</status>", prefixTestDataPath("xsd/simple_enum.xsd")).empty());
    REQUIRE(validateXSDFile("<status>pending</status>", prefixTestDataPath("xsd/simple_enum.xsd")).empty());
    const auto result = validateXSDFile("<status>deleted</status>", prefixTestDataPath("xsd/simple_enum.xsd"));
    REQUIRE(result.find("enumeration") != std::string::npos);
  }
  SECTION("minLength and maxLength restrictions enforced.", "[XML][XSD][Validate][SimpleTypes]")
  {
    REQUIRE(validateXSDFile("<username>alice</username>", prefixTestDataPath("xsd/simple_length.xsd")).empty());
    const auto tooShort = validateXSDFile("<username>ab</username>", prefixTestDataPath("xsd/simple_length.xsd"));
    REQUIRE(tooShort.find("minLength") != std::string::npos);
    const auto tooLong = validateXSDFile(
      "<username>averylongusernamethatexceedslimit</username>", prefixTestDataPath("xsd/simple_length.xsd"));
    REQUIRE(tooLong.find("maxLength") != std::string::npos);
  }
  SECTION("minExclusive restriction enforced.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"val\">\n"
      "    <xs:simpleType>\n"
      "      <xs:restriction base=\"xs:decimal\">\n"
      "        <xs:minExclusive value=\"0\"/>\n"
      "      </xs:restriction>\n"
      "    </xs:simpleType>\n"
      "  </xs:element>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<val>0.1</val>", xsd).empty());
    const auto result = validateXSD("<val>0</val>", xsd);
    REQUIRE(result.find("minExclusive") != std::string::npos);
  }
  SECTION("maxExclusive restriction enforced.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"val\">\n"
      "    <xs:simpleType>\n"
      "      <xs:restriction base=\"xs:decimal\">\n"
      "        <xs:maxExclusive value=\"10\"/>\n"
      "      </xs:restriction>\n"
      "    </xs:simpleType>\n"
      "  </xs:element>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<val>9.99</val>", xsd).empty());
    const auto result = validateXSD("<val>10</val>", xsd);
    REQUIRE(result.find("maxExclusive") != std::string::npos);
  }
  SECTION("Named simpleType validates attribute value.", "[XML][XSD][Validate][SimpleTypes]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"item\" type=\"ItemType\"/>\n"
      "  <xs:complexType name=\"ItemType\">\n"
      "    <xs:sequence/>\n"
      "    <xs:attribute name=\"priority\" type=\"PriorityType\"/>\n"
      "  </xs:complexType>\n"
      "  <xs:simpleType name=\"PriorityType\">\n"
      "    <xs:restriction base=\"xs:string\">\n"
      "      <xs:enumeration value=\"low\"/>\n"
      "      <xs:enumeration value=\"medium\"/>\n"
      "      <xs:enumeration value=\"high\"/>\n"
      "    </xs:restriction>\n"
      "  </xs:simpleType>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<item priority=\"high\"/>", xsd).empty());
    const auto result = validateXSD("<item priority=\"critical\"/>", xsd);
    REQUIRE(result.find("enumeration") != std::string::npos);
  }
}

TEST_CASE("XSD inline anonymous type validation.", "[XML][XSD][Validate][Inline]")
{
  SECTION("Inline complexType on element is validated.", "[XML][XSD][Validate][Inline]")
  {
    const std::string xsd{
      "<?xml version=\"1.0\"?>\n"
      "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
      "  <xs:element name=\"address\">\n"
      "    <xs:complexType>\n"
      "      <xs:sequence>\n"
      "        <xs:element name=\"street\" type=\"xs:string\"/>\n"
      "        <xs:element name=\"city\" type=\"xs:string\"/>\n"
      "      </xs:sequence>\n"
      "    </xs:complexType>\n"
      "  </xs:element>\n"
      "</xs:schema>\n"
    };
    REQUIRE(validateXSD("<address><street>123 Main St</street><city>Springfield</city></address>", xsd).empty());
    const auto result = validateXSD("<address><street>123 Main St</street></address>", xsd);
    REQUIRE(result.find("city") != std::string::npos);
  }
}
