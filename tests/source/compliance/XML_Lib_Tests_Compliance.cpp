#include "XML_Lib_Tests.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

TEST_CASE("W3C-derived XML parsing compliance", "[Compliance][XML]")
{
  SECTION("Valid simple XML fixture parses successfully.", "[Compliance][XML]")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(FileSource(prefixTestDataPath("w3c/xml/valid_simple.xml"))));
  }
}

#if defined(XML_LIB_ENABLE_DTD)
TEST_CASE("W3C-derived DTD validation compliance", "[Compliance][DTD]")
{
  SECTION("XML with external DTD subset validates successfully.", "[Compliance][DTD]")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(FileSource(prefixTestDataPath("w3c/dtd/valid_root.xml"))));
    REQUIRE_NOTHROW(xml.validate());
  }
}
#endif

#if defined(XML_LIB_ENABLE_XPATH)
TEST_CASE("W3C-derived XPath evaluation compliance", "[Compliance][XPath]")
{
  SECTION("XPath count and attribute filtering work as expected.", "[Compliance][XPath]")
  {
    XML xml(XML::fromFile(prefixTestDataPath("w3c/xpath/axis_example.xml")));
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("count(//item)") == "4");
    REQUIRE(xp.evaluateString("string(//item[@type='book'][1])") == "The Art of XML");
  }
}
#endif

#if defined(XML_LIB_ENABLE_XSD)
TEST_CASE("W3C-derived XSD validation compliance", "[Compliance][XSD]")
{
  SECTION("XSD schema validates a simple string element.", "[Compliance][XSD]")
  {
    XML xml(XML::fromFile(prefixTestDataPath("w3c/xsd/simple_string.xml")));
    REQUIRE_NOTHROW(xml.validate(XML::fromFile(prefixTestDataPath("w3c/xsd/simple_string.xsd"))));
  }
}
#endif

namespace {

struct W3CTestCase {
  std::string id;
  std::string type;       // "valid", "invalid", "not-wf", "error"
  std::string uri;
  std::filesystem::path fullPath;
  std::string version;    // "1.0", "1.1"
  std::string recommendation;
  std::string entities;
  std::string edition;
  std::string description;
  bool namespaceEnabled = true;
};

static std::filesystem::path findW3CXmlConfDirectory()
{
  // 1. Environment variables
  for (const char *var : { "XMLCONF_DIR", "W3C_XMLCONF_DIR", "XML_CONF_DIR" }) {
    if (const char *val = std::getenv(var); val && *val) {
      std::filesystem::path p{ val };
      if (std::filesystem::exists(p / "xmlconf.xml")) { return p; }
    }
  }

  // 2. Through prefixTestDataPath
  for (const std::string &rel : { "xmlconf", "w3c/xmlconf" }) {
    std::filesystem::path p = prefixTestDataPath(rel);
    if (std::filesystem::exists(p / "xmlconf.xml")) { return p; }
    std::filesystem::path pXml = prefixTestDataPath(rel + "/xmlconf.xml");
    if (std::filesystem::exists(pXml)) { return pXml.parent_path(); }
  }

  // 3. Common relative directories
  const auto repoTestsFiles = std::filesystem::path(__FILE__).parent_path().parent_path() / "files";
  for (const std::filesystem::path &p : {
         std::filesystem::current_path() / "xmlconf",
         std::filesystem::current_path() / "files" / "xmlconf",
         std::filesystem::current_path() / "tests" / "files" / "xmlconf",
         std::filesystem::current_path() / "tests" / "files" / "w3c" / "xmlconf",
         repoTestsFiles / "xmlconf",
         repoTestsFiles / "w3c" / "xmlconf",
       }) {
    if (std::filesystem::exists(p / "xmlconf.xml")) { return p; }
  }

  return {};
}

static std::string readFileToString(const std::filesystem::path &p)
{
  std::ifstream f(p, std::ios::binary);
  if (!f) return {};
  std::ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

static std::vector<W3CTestCase> loadW3CCatalog(const std::filesystem::path &xmlconfDir)
{
  std::vector<W3CTestCase> testCases;
  const auto masterCatalog = xmlconfDir / "xmlconf.xml";
  const std::string content = readFileToString(masterCatalog);
  if (content.empty()) return testCases;

  // Extract entity mappings: <!ENTITY <name> SYSTEM "path"> or 'path'
  std::map<std::string, std::string> entities;
  std::regex entRegex(R"(<!ENTITY\s+([\w-]+)\s+SYSTEM\s+['"]([^'"]+)['"]>)");
  auto entBegin = std::sregex_iterator(content.begin(), content.end(), entRegex);
  auto entEnd = std::sregex_iterator();
  for (auto it = entBegin; it != entEnd; ++it) {
    entities[(*it)[1].str()] = (*it)[2].str();
  }

  // Extract TESTCASES blocks: <TESTCASES ...> ... </TESTCASES>
  std::regex testcasesRegex(R"(<TESTCASES\b([^>]*)>([\s\S]*?)</TESTCASES>)");
  std::regex baseAttrRegex(R"(xml:base\s*=\s*['"]([^'"]*)['"])");
  std::regex refRegex(R"(&([\w-]+);)");
  std::regex testRegex(R"(<TEST\b([^>]*)(?:>([\s\S]*?)</TEST>|/>))");
  std::regex attrRegex(R"(([\w:]+)\s*=\s*['"]([^'"]*)['"])");

  auto tcBegin = std::sregex_iterator(content.begin(), content.end(), testcasesRegex);
  auto tcEnd = std::sregex_iterator();

  for (auto it = tcBegin; it != tcEnd; ++it) {
    std::string tagAttrs = (*it)[1].str();
    std::string body = (*it)[2].str();

    std::string xmlBase;
    std::smatch baseMatch;
    if (std::regex_search(tagAttrs, baseMatch, baseAttrRegex)) {
      xmlBase = baseMatch[1].str();
    }

    auto refBegin = std::sregex_iterator(body.begin(), body.end(), refRegex);
    auto refEnd = std::sregex_iterator();
    for (auto rIt = refBegin; rIt != refEnd; ++rIt) {
      std::string entName = (*rIt)[1].str();
      auto entIt = entities.find(entName);
      if (entIt == entities.end()) continue;

      std::filesystem::path subFile = xmlconfDir / entIt->second;
      std::string subContent = readFileToString(subFile);
      if (subContent.empty()) continue;

      auto testBegin = std::sregex_iterator(subContent.begin(), subContent.end(), testRegex);
      auto testEnd = std::sregex_iterator();
      for (auto tIt = testBegin; tIt != testEnd; ++tIt) {
        std::string attrsStr = (*tIt)[1].str();
        std::string desc = (*tIt)[2].str();

        std::map<std::string, std::string> attrs;
        auto aBegin = std::sregex_iterator(attrsStr.begin(), attrsStr.end(), attrRegex);
        auto aEnd = std::sregex_iterator();
        for (auto aIt = aBegin; aIt != aEnd; ++aIt) {
          attrs[(*aIt)[1].str()] = (*aIt)[2].str();
        }

        W3CTestCase tc;
        tc.id = attrs["ID"];
        tc.type = attrs["TYPE"];
        tc.uri = attrs["URI"];
        tc.version = attrs.count("VERSION") ? attrs["VERSION"] : "1.0";
        tc.recommendation = attrs.count("RECOMMENDATION") ? attrs["RECOMMENDATION"] : "XML1.0";
        tc.entities = attrs.count("ENTITIES") ? attrs["ENTITIES"] : "none";
        tc.edition = attrs.count("EDITION") ? attrs["EDITION"] : "";
        tc.namespaceEnabled = attrs.count("NAMESPACE") ? (attrs["NAMESPACE"] != "no") : true;
        tc.description = desc;
        tc.fullPath = xmlconfDir / xmlBase / tc.uri;

        testCases.push_back(std::move(tc));
      }
    }
  }

  return testCases;
}

} // anonymous namespace

TEST_CASE("Official W3C XML Conformance Test Suite", "[Compliance][W3C][XMLConf]")
{
  const auto xmlconfDir = findW3CXmlConfDirectory();
  if (xmlconfDir.empty()) {
    SUCCEED("Official W3C XML Conformance Test Suite (xmlconf) not present; skipping.");
    return;
  }

  const auto testCases = loadW3CCatalog(xmlconfDir);
  REQUIRE_FALSE(testCases.empty());

  size_t testedCount = 0;
  size_t failedCount = 0;

  for (const auto &tc : testCases) {
    // Only XML 1.0 Fifth Edition tests (XML_Lib is an XML 1.0 Fifth Edition processor)
    if (tc.version == "1.1") {
      continue;
    }
    // Skip tests for editions not supported (per testcases.dtd: "Parsers should not run tests for editions they do not support")
    if (!tc.edition.empty() && tc.edition.find('5') == std::string::npos) {
      continue;
    }
    // Parsers are not required to report errors for TYPE="error" per W3C spec
    if (tc.type == "error") {
      continue;
    }
    if (!std::filesystem::exists(tc.fullPath)) {
      continue;
    }

    ++testedCount;

    try {
      XML xml;
      ParseOptions options;
      options.allowExternalEntities = true;
      options.maxEntityExpansionDepth = 4096;
      options.strictNamespaces = true;
      options.enableNamespaces = tc.namespaceEnabled;
      options.allowFuture1xVersions = true;
      options.firstEntityDeclarationBinding = true;
      xml.parse(tc.fullPath, options);

      // If parsing succeeded without error:
      if (tc.type == "not-wf") {
        ++failedCount;
        FAIL_CHECK("W3C XML Conformance failure: Test ID '" << tc.id
                   << "' expected NOT well-formed (TYPE='not-wf'), but parsed successfully. File: "
                   << tc.fullPath.string());
      }
    } catch (const XML_Lib::Error &e) {
      if (tc.type == "valid") {
        ++failedCount;
        FAIL_CHECK("W3C XML Conformance failure: Test ID '" << tc.id
                   << "' expected valid (TYPE='valid'), but failed with exception: "
                   << e.what() << ". File: " << tc.fullPath.string());
      }
    } catch (const std::exception &e) {
      if (tc.type == "valid") {
        ++failedCount;
        FAIL_CHECK("W3C XML Conformance failure: Test ID '" << tc.id
                   << "' expected valid (TYPE='valid'), but failed with std::exception: "
                   << e.what() << ". File: " << tc.fullPath.string());
      }
    }
  }

  INFO("W3C XML Conformance Tests evaluated: " << testedCount << ", Failures: " << failedCount);
  REQUIRE(failedCount == 0);
}
