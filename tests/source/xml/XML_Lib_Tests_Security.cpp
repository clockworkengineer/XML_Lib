#include "XML_Lib_Tests.hpp"
#include "IEntityResolver.hpp"

// ============================================================
// ParseOptions security tests (§7 hardening coverage)
// ============================================================

using namespace Catch::Matchers;

// ============================================================
// Attribute count limit (§7.5)
// ============================================================

TEST_CASE("ParseOptions security: attribute count limit", "[XML][Security][Attributes]")
{
  SECTION("Parse succeeds when attribute count equals the limit")
  {
    ParseOptions opts;
    opts.maxAttributeCount = 3;
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ R"(<root a="1" b="2" c="3"/>)" }, opts));
  }
  SECTION("Parse throws SyntaxError when attribute count exceeds the limit")
  {
    ParseOptions opts;
    opts.maxAttributeCount = 3;
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ R"(<root a="1" b="2" c="3" d="4"/>)" }, opts),
      ContainsSubstring("Maximum attribute count exceeded."));
  }
  SECTION("Default limit (10000) allows ordinary XML")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ R"(<root a="1" b="2" c="3" d="4" e="5"/>)" }, {}));
  }
  SECTION("maxAttributeCount = 0 throws on any attribute")
  {
    ParseOptions opts;
    opts.maxAttributeCount = 0;
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ R"(<root a="1"/>)" }, opts),
      ContainsSubstring("Maximum attribute count exceeded."));
  }
}

// ============================================================
// Element nesting depth limit (§7.2)
// ============================================================

TEST_CASE("ParseOptions security: element nesting depth limit", "[XML][Security][Nesting]")
{
  SECTION("Parse succeeds when nesting is exactly at the limit")
  {
    // maxNestingDepth = 3: root(1) → a(2) → b(3-opens but not checked until interior)
    // root opens at depth 0 → increments to 1; a at 1→2; b at 2>=3 FALSE → depth=3; OK
    ParseOptions opts;
    opts.maxNestingDepth = 3;
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ "<root><a><b></b></a></root>" }, opts));
  }
  SECTION("Parse throws when nesting depth exceeds the limit")
  {
    // root(depth 0→1), a(1→2), b: depth 2>=2 TRUE → throws
    ParseOptions opts;
    opts.maxNestingDepth = 2;
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ "<root><a><b></b></a></root>" }, opts),
      ContainsSubstring("Maximum element nesting depth exceeded."));
  }
  SECTION("Self-closing elements do not consume nesting depth")
  {
    ParseOptions opts;
    opts.maxNestingDepth = 2;
    XML xml;
    // root(0→1), then a/ b/ c/ are Self nodes — no depth increment
    REQUIRE_NOTHROW(xml.parse(BufferSource{ "<root><a/><b/><c/></root>" }, opts));
  }
  SECTION("maxNestingDepth = 1 allows a root-only document")
  {
    ParseOptions opts;
    opts.maxNestingDepth = 1;
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ "<root/>" }, opts));
  }
  SECTION("maxNestingDepth = 1 rejects any child element")
  {
    ParseOptions opts;
    opts.maxNestingDepth = 1;
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ "<root><child></child></root>" }, opts),
      ContainsSubstring("Maximum element nesting depth exceeded."));
  }
  SECTION("Deeply nested XML triggers limit")
  {
    // Build <root><a><a>...</a></a></root> with 10 nested <a> elements
    std::string deep = "<root>";
    for (int i = 0; i < 10; ++i) deep += "<a>";
    for (int i = 0; i < 10; ++i) deep += "</a>";
    deep += "</root>";

    ParseOptions opts;
    opts.maxNestingDepth = 5;
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(BufferSource{ deep }, opts),
      ContainsSubstring("Maximum element nesting depth exceeded."));
  }
}

// ============================================================
// External entity denial (§7.3 — XXE defence)
// §7.3 and §6.2 require DTD support to define external entities
// ============================================================

#if defined(XML_LIB_ENABLE_DTD)

static const std::string kExternalEntityXml =
  "<?xml version=\"1.0\"?>\n"
  "<!DOCTYPE root [\n"
  "  <!ENTITY ext SYSTEM \"nonexistent_external.xml\">\n"
  "]>\n"
  "<root>&ext;</root>";

TEST_CASE("ParseOptions security: external entity default denial (XXE)", "[XML][Security][XXE]")
{
  SECTION("External entity reference throws with default ParseOptions")
  {
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ kExternalEntityXml }),
      ContainsSubstring("External entity resolution is disabled."));
  }
  SECTION("External entity reference throws when allowExternalEntities = false and no resolver")
  {
    ParseOptions opts;
    opts.allowExternalEntities = false;
    opts.entityResolver        = nullptr;
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ kExternalEntityXml }, opts),
      ContainsSubstring("External entity resolution is disabled."));
  }
}

TEST_CASE("ParseOptions security: external entity via IEntityResolver", "[XML][Security][XXE][Resolver]")
{
  // Minimal resolver that always returns a fixed content string
  struct AlwaysResolve : IEntityResolver {
    [[nodiscard]] std::optional<std::string>
    resolve(const std::string_view & /*systemId*/, const std::string_view & /*publicId*/) override
    {
      return "resolved-content";
    }
  };

  // Resolver that denies every request (returns nullopt)
  struct AlwaysDeny : IEntityResolver {
    [[nodiscard]] std::optional<std::string>
    resolve(const std::string_view & /*systemId*/, const std::string_view & /*publicId*/) override
    {
      return std::nullopt;
    }
  };

  SECTION("IEntityResolver returning content allows parsing without throwing")
  {
    AlwaysResolve resolver;
    ParseOptions  opts;
    opts.entityResolver = &resolver;
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ kExternalEntityXml }, opts));
  }
  SECTION("IEntityResolver returning nullopt propagates as SyntaxError")
  {
    AlwaysDeny   resolver;
    ParseOptions opts;
    opts.entityResolver = &resolver;
    XML xml;
    // nullopt → falls through to filesystem → file doesn't exist → SyntaxError
    REQUIRE_THROWS(xml.parse(BufferSource{ kExternalEntityXml }, opts));
  }
  SECTION("allowExternalEntities = true resolves from filesystem when file exists")
  {
    // Write a temporary XML fragment file and parse it
    const std::string fileName    = generateRandomFileName();
    const std::string fragContent = "<item>resolved</item>";
    XML::toFile(fileName, fragContent, XML::Format::utf8);

    // Build XML that references this file as an external entity
    const std::string xmlWithExt =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "  <!ENTITY ext SYSTEM \"" + fileName + "\">\n"
      "]>\n"
      "<root>&ext;</root>";

    ParseOptions opts;
    opts.allowExternalEntities = true;
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ xmlWithExt }, opts));

    std::filesystem::remove(fileName);
  }
}

// ============================================================
// Entity expansion depth limit (§7.1)
// ============================================================

TEST_CASE("ParseOptions security: entity expansion depth limit", "[XML][Security][Entity]")
{
  // Entity chain: &outer; → text containing &inner; → "inner-value"
  // Depth: outer is at depth 0; inner is parsed at depth 1.
  static const std::string kNestedEntityXml =
    "<?xml version=\"1.0\"?>\n"
    "<!DOCTYPE root [\n"
    "  <!ENTITY inner \"inner-value\">\n"
    "  <!ENTITY outer \"&inner;\">\n"
    "]>\n"
    "<root>&outer;</root>";

  SECTION("Expansion succeeds when depth limit accommodates the chain")
  {
    ParseOptions opts;
    opts.maxEntityExpansionDepth = 2; // outer(0) + inner(1) — both allowed
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ kNestedEntityXml }, opts));
  }
  SECTION("Expansion throws when depth limit is too shallow for the chain")
  {
    ParseOptions opts;
    opts.maxEntityExpansionDepth = 1; // outer(0) ok, inner at depth 1 >= 1 → throws
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ kNestedEntityXml }, opts),
      ContainsSubstring("Entity expansion depth limit exceeded."));
  }
  SECTION("maxEntityExpansionDepth = 0 rejects any entity expansion")
  {
    static const std::string kSimpleEntityXml =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "  <!ENTITY val \"hello\">\n"
      "]>\n"
      "<root>&val;</root>";

    ParseOptions opts;
    opts.maxEntityExpansionDepth = 0;
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(BufferSource{ kSimpleEntityXml }, opts),
      ContainsSubstring("Entity expansion depth limit exceeded."));
  }
  SECTION("Default depth limit allows typical entity use")
  {
    static const std::string kSimpleEntityXml =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "  <!ENTITY greet \"Hello, World!\">\n"
      "]>\n"
      "<root>&greet;</root>";

    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ kSimpleEntityXml }, {}));
  }
}

#endif // XML_LIB_ENABLE_DTD
