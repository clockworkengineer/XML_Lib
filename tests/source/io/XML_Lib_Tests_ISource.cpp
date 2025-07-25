#include "XML_Lib_Tests.hpp"

TEST_CASE("ISource (File) interface.", "[XML][FileSource]")
{
  SECTION("Create FileSource with testfile001.xml.", "[XML][FileSource]")
  {
    REQUIRE_NOTHROW(FileSource(prefixTestDataPath(kSingleXMLFile)));
  }
  SECTION("Create FileSource with empty.xml.", "[XML][FileSource]")
  {
    REQUIRE_NOTHROW(FileSource(prefixTestDataPath(KEmptyXMLFile)));
  }
  SECTION("Create FileSource with non existent file.", "[XML][FileSource][Exception]")
  {
    REQUIRE_THROWS_AS(FileSource(prefixTestDataPath(kNonExistantXMLFile)), FileSource::Error);
    REQUIRE_THROWS_WITH(FileSource(prefixTestDataPath(kNonExistantXMLFile)),
      "FileSource Error: File input stream failed to open or does not exist.");
  }
  SECTION("Create FileSource with testfile001.xml. and positioned on the correct first character", "[XML][FileSource]")
  {
    FileSource source{ prefixTestDataPath(kSingleXMLFile) };
    REQUIRE_FALSE(!source.more());
    REQUIRE(source.current() == '<');
  }
  SECTION("Create FileSource with testfile001.xml and then check move positions to correct next character",
    "[XML][FileSource]")
  {
    FileSource source{ prefixTestDataPath(kSingleXMLFile) };
    source.next();
    REQUIRE_FALSE(!source.more());
    REQUIRE(source.current() == '?');
  }
  SECTION("Create FileSource with testfile001.xml  move past last character, check it and the bytes moved.",
    "[XML][FileSource]")
  {
    FileSource source{ prefixTestDataPath(kSingleXMLFile) };
    long length = 0;
    while (source.more()) {
      source.next();
      length++;
    }
    REQUIRE(length == 8752);// eof
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));// eof
  }
  std::string xmlString;
  SECTION(
    "Check that FileSource is  performing CRLF to LF conversion on windows format data correctly.", "[XML][FileSource]")
  {
    xmlString =
      "\r\r\n<!DOCTYPE REPORT [\r\n"
      "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\r\n"
      "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
      "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
      "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\r\n"
      "<!ELEMENT TITLE %TEXT;>\r\n"
      "<!ELEMENT PARA %TEXT;>\r\n"
      "<!ELEMENT LIST (ITEM)+>\r\n"
      "<!ELEMENT ITEM (%BLOCK;)>\r\n"
      "<!ELEMENT CODE (#PCDATA)>\r\n"
      "<!ELEMENT KEYWORD (#PCDATA)>\r\n"
      "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\r\n"
      "<!ELEMENT GRAPHIC EMPTY>\r\n"
      "<!ATTLIST REPORT security (high | medium | low ) \"low\">\r\n"
      "<!ATTLIST CODE type CDATA #IMPLIED>\r\n"
      "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\r\n"
      "<!ENTITY xml \"Extensible Markup Language\">\r\n"
      "<!ENTITY sgml \"Standard Generalized Markup Language\">\r\n"
      "<!ENTITY pxa \"Professional XML Authoring\">\r\n"
      "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\r\n"
      "<!ENTITY % BLOCK \"(PARA|LIST)+\">\r\n"
      "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\r\n"
      "<!NOTATION GIF SYSTEM \"\">\r\n"
      "<!NOTATION JPG SYSTEM \"\">\r\n"
      "<!NOTATION BMP SYSTEM \"\">\r\n"
      "]>\r\n"
      "<REPORT>\r\r </REPORT>\r\n";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    verifyCRLFCount(source, 28, 3);
    source.close();
    std::filesystem::remove(generatedFileName);
  }
  SECTION(
    "Check that FileSource is  performing CRLF to LF conversion on linux format data correctly.", "[XML][FileSource]")
  {
    xmlString =
      "\r \n<!DOCTYPE REPORT [\n"
      "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\n"
      "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\n"
      "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\n"
      "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\n"
      "<!ELEMENT TITLE %TEXT;>\n"
      "<!ELEMENT PARA %TEXT;>\n"
      "<!ELEMENT LIST (ITEM)+>\n"
      "<!ELEMENT ITEM (%BLOCK;)>\n"
      "<!ELEMENT CODE (#PCDATA)>\n"
      "<!ELEMENT KEYWORD (#PCDATA)>\n"
      "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\n"
      "<!ELEMENT GRAPHIC EMPTY>\n"
      "<!ATTLIST REPORT security (high | medium | low ) \"low\">\n"
      "<!ATTLIST CODE type CDATA #IMPLIED>\n"
      "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\n"
      "<!ENTITY xml \"Extensible Markup Language\">\n"
      "<!ENTITY sgml \"Standard Generalized Markup Language\">\n"
      "<!ENTITY pxa \"Professional XML Authoring\">\n"
      "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\n"
      "<!ENTITY % BLOCK \"(PARA|LIST)+\">\n"
      "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\n"
      "<!NOTATION GIF SYSTEM \"\">\n"
      "<!NOTATION JPG SYSTEM \"\">\n"
      "<!NOTATION BMP SYSTEM \"\">\n"
      "]>\n"
      "<REPORT>\r\r </REPORT>\n";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    verifyCRLFCount(source, 28, 3);
    source.close();
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Check that FileSource is ignoring whitespace correctly.", "[XML][FileSource]")
  {
    xmlString = "<root>   Test\t\t\t\r\r\r\r\r\r\r\f\n       Test       Test   \r\r\r\r</root>";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    String xmlResult;
    while (source.more()) {
      source.ignoreWS();
      xmlResult += source.current();
      source.next();
    }
    REQUIRE(xmlResult == u"<root>TestTestTest</root>");
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));
    source.close();
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Check that FileSource ignoreWS() at end of file does not throw but next() does.", "[XML][FileSource]")
  {
    xmlString = "<root>Test Test Test Test</root>";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    while (source.more()) { source.next(); }
    REQUIRE_NOTHROW(source.ignoreWS());
    REQUIRE_THROWS_AS(source.next(), FileSource::Error);
    REQUIRE_THROWS_WITH(source.next(), "FileSource Error: Parse buffer empty before parse complete.");
    source.close();
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Check that FileSource match works correctly when match found or not.", "[XML][FileSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    REQUIRE_FALSE(source.match("<root> "));
    REQUIRE_FALSE(!source.match("<root>"));
    REQUIRE(source.current() == 'M');
    REQUIRE_FALSE(!source.match("Match1"));
    REQUIRE(source.current() == ' ');
    source.ignoreWS();
    REQUIRE(source.current() == 'M');
    REQUIRE_FALSE(source.match("Match3"));
    REQUIRE_FALSE(!source.match("Match2"));
    source.ignoreWS();
    REQUIRE(source.current() == '2');
    REQUIRE_FALSE(!source.match("2hctam"));
    REQUIRE(source.current() == ' ');
    source.ignoreWS();
    REQUIRE_FALSE(!source.match("MMAATTCCHHHXML_Lib"));
    REQUIRE(source.current() == ' ');
    source.next();
    REQUIRE(source.current() == '&');
    source.next();
    REQUIRE_FALSE(!source.match("</root>"));
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));
    REQUIRE_THROWS_WITH(source.next(), "FileSource Error: Parse buffer empty before parse complete.");
    source.close();
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Check that FileSource backup works and doesn't go negative.", "[XML][FileSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    source.match("<root>Match1");
    REQUIRE(source.current() == ' ');
    source.backup(12);
    REQUIRE(source.current() == '<');
    source.next();
    REQUIRE(source.current() == 'r');
    source.backup(12);
    REQUIRE(source.current() == '<');
    while (source.more()) { source.next(); }
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));
    source.backup(1);
    REQUIRE(source.current() == '>');
    source.close();
    std::filesystem::remove(generatedFileName);
  }
}
TEST_CASE("ISource (Buffer) interface (buffer contains file testfile001.xml).", "[XML][BufferSource]")
{
  std::string buffer{ XML::fromFile(prefixTestDataPath(kSingleXMLFile)) };
  SECTION("Create BufferSource.", "[XML][BufferSource]") { REQUIRE_NOTHROW(BufferSource(buffer)); }
  SECTION("Create BufferSource with empty buffer.", "[XML][BufferSource][Exception]")
  {
    REQUIRE_THROWS_AS(BufferSource(""), BufferSource::Error);
    REQUIRE_THROWS_WITH(BufferSource(""), "BufferSource Error: Empty source buffer passed to be parsed.");
  }
  SECTION("Create BufferSource with testfile001.xml and that it is positioned on the correct first character",
    "[XML][BufferSource]")
  {
    BufferSource source(buffer);
    REQUIRE_FALSE(!source.more());
    REQUIRE((char)source.current() == '<');
  }
  SECTION("Create BufferSource with testfile001.xml and then check more positions to correct next character",
    "[XML][BufferSource]")
  {
    BufferSource source(buffer);
    source.next();
    REQUIRE_FALSE(!source.more());
    REQUIRE((char)source.current() == '?');
  }
  SECTION("Create BufferSource with testfile001.xml move past last character, check it and the bytes moved.",
    "[XML][BufferSource]")
  {
    BufferSource source(buffer);
    long length = 0;
    while (source.more()) {
      source.next();
      length++;
    }
    REQUIRE(length == 8752);// eof
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));// eof
  }
  std::string xmlString;
  SECTION("Check that BufferSource is  performing CRLF to LF conversion on windows format data correctly.",
    "[XML][BufferSource]")
  {
    xmlString =
      "\r\r\n<!DOCTYPE REPORT [\r\n"
      "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\r\n"
      "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
      "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\r\n"
      "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\r\n"
      "<!ELEMENT TITLE %TEXT;>\r\n"
      "<!ELEMENT PARA %TEXT;>\r\n"
      "<!ELEMENT LIST (ITEM)+>\r\n"
      "<!ELEMENT ITEM (%BLOCK;)>\r\n"
      "<!ELEMENT CODE (#PCDATA)>\r\n"
      "<!ELEMENT KEYWORD (#PCDATA)>\r\n"
      "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\r\n"
      "<!ELEMENT GRAPHIC EMPTY>\r\n"
      "<!ATTLIST REPORT security (high | medium | low ) \"low\">\r\n"
      "<!ATTLIST CODE type CDATA #IMPLIED>\r\n"
      "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\r\n"
      "<!ENTITY xml \"Extensible Markup Language\">\r\n"
      "<!ENTITY sgml \"Standard Generalized Markup Language\">\r\n"
      "<!ENTITY pxa \"Professional XML Authoring\">\r\n"
      "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\r\n"
      "<!ENTITY % BLOCK \"(PARA|LIST)+\">\r\n"
      "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\r\n"
      "<!NOTATION GIF SYSTEM \"\">\r\n"
      "<!NOTATION JPG SYSTEM \"\">\r\n"
      "<!NOTATION BMP SYSTEM \"\">\r\n"
      "]>\r\n"
      "<REPORT>\r\r </REPORT>\r\n";
    BufferSource source{ xmlString };
    verifyCRLFCount(source, 28, 3);
  }
  SECTION("Check that BufferSource is  performing CRLF to LF conversion on linux format data correctly.",
    "[XML][BufferSource]")
  {
    xmlString =
      "\r \n<!DOCTYPE REPORT [\n"
      "<!ELEMENT REPORT (TITLE,(SECTION|SHORTSECT)+)>\n"
      "<!ELEMENT SECTION (TITLE,%BODY;,SUBSECTION*)>\n"
      "<!ELEMENT SUBSECTION (TITLE,%BODY;,SUBSECTION*)>\n"
      "<!ELEMENT SHORTSECT (TITLE,%BODY;)>\n"
      "<!ELEMENT TITLE %TEXT;>\n"
      "<!ELEMENT PARA %TEXT;>\n"
      "<!ELEMENT LIST (ITEM)+>\n"
      "<!ELEMENT ITEM (%BLOCK;)>\n"
      "<!ELEMENT CODE (#PCDATA)>\n"
      "<!ELEMENT KEYWORD (#PCDATA)>\n"
      "<!ELEMENT EXAMPLE (TITLE?,%BLOCK;)>\n"
      "<!ELEMENT GRAPHIC EMPTY>\n"
      "<!ATTLIST REPORT security (high | medium | low ) \"low\">\n"
      "<!ATTLIST CODE type CDATA #IMPLIED>\n"
      "<!ATTLIST GRAPHIC file ENTITY #REQUIRED>\n"
      "<!ENTITY xml \"Extensible Markup Language\">\n"
      "<!ENTITY sgml \"Standard Generalized Markup Language\">\n"
      "<!ENTITY pxa \"Professional XML Authoring\">\n"
      "<!ENTITY % TEXT \"(#PCDATA|CODE|KEYWORD|QUOTATION)*\">\n"
      "<!ENTITY % BLOCK \"(PARA|LIST)+\">\n"
      "<!ENTITY % BODY \"(%BLOCK;|EXAMPLE|NOTE)+\">\n"
      "<!NOTATION GIF SYSTEM \"\">\n"
      "<!NOTATION JPG SYSTEM \"\">\n"
      "<!NOTATION BMP SYSTEM \"\">\n"
      "]>\n"
      "<REPORT>\r\r </REPORT>\n";
    BufferSource source{ xmlString };
    verifyCRLFCount(source, 28, 3);
  }
  SECTION("Check that BufferSource is ignoring whitespace correctly.", "[XML][BufferSource]")
  {
    xmlString = "<root>   Test\t\t\t\r\r\r\r\r\r\r\f\n       Test       Test   \r\r\r\r</root>";
    BufferSource source{ xmlString };
    String xmlResult;
    while (source.more()) {
      source.ignoreWS();
      xmlResult += source.current();
      source.next();
    }
    REQUIRE(xmlResult == u"<root>TestTestTest</root>");
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));
  }
  SECTION("Check that BufefrSource ignoreWS() at end of file does not throw but next() does.", "[XML][BufferSource]")
  {
    xmlString = "<root>Test Test Test Test</root>";
    BufferSource source{ xmlString };
    while (source.more()) { source.next(); }
    REQUIRE_NOTHROW(source.ignoreWS());
    REQUIRE_THROWS_AS(source.next(), BufferSource::Error);
    REQUIRE_THROWS_WITH(source.next(), "BufferSource Error: Parse buffer empty before parse complete.");
  }
  SECTION("Check that BufferSource match works correctly when match found or not.", "[XML][BufferSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    BufferSource source{ xmlString };
    REQUIRE_FALSE(source.match("<root> "));
    REQUIRE_FALSE(!source.match("<root>"));
    REQUIRE(source.current() == 'M');
    REQUIRE_FALSE(!source.match("Match1"));
    REQUIRE(source.current() == ' ');
    source.ignoreWS();
    REQUIRE(source.current() == 'M');
    REQUIRE_FALSE(source.match("Match3"));
    REQUIRE_FALSE(!source.match("Match2"));
    source.ignoreWS();
    REQUIRE(source.current() == '2');
    REQUIRE_FALSE(!source.match("2hctam"));
    REQUIRE(source.current() == ' ');
    source.ignoreWS();
    REQUIRE_FALSE(!source.match("MMAATTCCHHHXML_Lib"));
    REQUIRE(source.current() == ' ');
    source.next();
    REQUIRE(source.current() == '&');
    source.next();
    REQUIRE_FALSE(!source.match("</root>"));
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));
    REQUIRE_THROWS_WITH(source.next(), "BufferSource Error: Parse buffer empty before parse complete.");
  }
  SECTION("Check that BufferSource backup works and doesn't go negative.", "[XML][BufferSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    BufferSource source{ xmlString };
    source.match("<root>Match1");
    REQUIRE(source.current() == ' ');
    source.backup(12);
    REQUIRE(source.current() == '<');
    source.backup(12);
    REQUIRE(source.current() == '<');
    while (source.more()) { source.next(); }
    REQUIRE(source.current() == static_cast<XML_Lib::Char>(EOF));
    source.backup(1);
    REQUIRE(source.current() == '>');
  }
  SECTION("Check that BufferSource is reporting correct line/column number in an error.", "[XML][BufferSource]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE queue ["
      "<!ELEMENT queue (person)+>\n"
      "<!ELEMENT person (firstName, lastName, nationality)>\n"
      "<!ELEMENT firstName (#PCDATA)>\n"
      "<!ELEMENT lastName (#PCDATA)>\n"
      "<!ELEMENT nationality (#PCDATA)\n"
      "<!ATTLIST person gender ( M | F | F) \"M\">\n"
      "]>\n"
      "<queue>\n"
      "<person "
      "gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></"
      "person>\n"
      "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 7 Column: 2] Missing '>' terminator.");
  }
  SECTION("Check that BufferSource position() and getRange works correctly.", "[XML][BufferSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    BufferSource source{ xmlString };
    while (source.more() && !source.match("Match")) { source.next(); }
    REQUIRE(source.position() == 11);
    long start = source.position();
    while (source.more() && !source.match("Match2")) { source.next(); }
    REQUIRE(source.position() == 22);
    REQUIRE(source.getRange(start, source.position()) == "1    Match2");
    REQUIRE(source.position() == 22);
  }
  SECTION("Check that FileSource position() and getRange works correctly.", "[XML][FileSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    while (source.more() && !source.match("Match")) { source.next(); }
    REQUIRE(source.position() == 11);
    long start = source.position();
    while (source.more() && !source.match("Match2")) { source.next(); }
    REQUIRE(source.position() == 22);
    REQUIRE(source.getRange(start, source.position()) == "1    Match2");
    REQUIRE(source.position() == 22);
    source.close();
    std::filesystem::remove(generatedFileName);
  }

  SECTION("Check that BufferSource reset() works correctly.", "[XML][BufferSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    BufferSource source{ xmlString };
    while (source.more() && !source.match("Match")) { source.next(); }
    REQUIRE(source.position() == 11);
    source.reset();
    REQUIRE(source.position() == 0);
    while (source.more() && !source.match("Match")) { source.next(); }
    REQUIRE(source.position() == 11);
  }
  SECTION("Check that FileSource reset() works correctly.", "[XML][FileSource]")
  {
    xmlString = "<root>Match1    Match2 2hctam        MMAATTCCHHHXML_Lib &</root>";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    FileSource source{ generatedFileName };
    while (source.more() && !source.match("Match")) { source.next(); }
    REQUIRE(source.position() == 11);
    source.reset();
    REQUIRE(source.position() == 0);
    while (source.more() && !source.match("Match")) { source.next(); }
    REQUIRE(source.position() == 11);
    source.close();
    std::filesystem::remove(generatedFileName);
  }
}
