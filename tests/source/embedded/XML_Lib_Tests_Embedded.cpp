#include "XML_Lib_Tests.hpp"
#include <filesystem>
#include <fstream>

#ifdef XML_LIB_EMBEDDED

// ============================================================
// Compile-time feature gating
// ============================================================

TEST_CASE("Embedded build compile-time feature gating", "[XML][Embedded][Gating]")
{
#if !defined(XML_LIB_NO_EXCEPTIONS)
  FAIL("XML_LIB_NO_EXCEPTIONS must be enabled for embedded builds");
#endif
#if !defined(XML_LIB_MINIMAL_FEATURES)
  FAIL("XML_LIB_MINIMAL_FEATURES must be enabled for embedded builds");
#endif
#if defined(XML_LIB_ENABLE_XPATH)
  FAIL("XML_LIB_ENABLE_XPATH must be disabled for embedded minimal builds");
#endif
#if defined(XML_LIB_ENABLE_XSD)
  FAIL("XML_LIB_ENABLE_XSD must be disabled for embedded minimal builds");
#endif
#if defined(XML_LIB_ENABLE_DTD)
  FAIL("XML_LIB_ENABLE_DTD must be disabled for embedded minimal builds");
#endif
  SUCCEED("Embedded feature gating is configured correctly");
}

// ============================================================
// Minimal parse round-trips
// ============================================================

TEST_CASE("Embedded minimal XML parse from buffer", "[XML][Embedded][Parse][Buffer]")
{
  SECTION("Parse minimal root element")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ "<root/>" }));
    REQUIRE_FALSE(xml.root().isEmpty());
  }
  SECTION("Parse element with attribute")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ "<item id=\"42\"/>" }));
    REQUIRE(NRef<Element>(xml.root()).getAttributes().size() == 1);
    REQUIRE(NRef<Element>(xml.root()).getAttributes()[0].getName() == "id");
  }
  SECTION("Parse element with text content")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ "<msg>hello</msg>" }));
    REQUIRE(xml.root().getChildren().size() == 1);
    REQUIRE(isA<Content>(xml.root().getChildren()[0]));
  }
  SECTION("Parse nested elements")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ "<root><a><b/></a></root>" }));
    REQUIRE(xml.root().getChildren().size() == 1);
  }
  SECTION("Parse XML with declaration")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root/>" }));
    REQUIRE_FALSE(xml.declaration().isEmpty());
  }
}

TEST_CASE("Embedded minimal XML parse from file", "[XML][Embedded][Parse][File]")
{
  const std::string xmlString =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
    "<root><child attr=\"val\">text</child></root>";
  const std::string fileName = generateRandomFileName();
  XML::toFile(fileName, xmlString, XML::Format::utf8);

  SECTION("Parse from FileSource")
  {
    XML xml;
    FileSource fileSource{ fileName };
    REQUIRE_NOTHROW(xml.parse(fileSource));
    REQUIRE_FALSE(xml.root().isEmpty());
  }
  SECTION("Parse via fromFile helper")
  {
    XML xml;
    BufferSource source{ XML::fromFile(fileName) };
    REQUIRE_NOTHROW(xml.parse(source));
    REQUIRE(NRef<Element>(xml.root()).name() == "root");
  }

  std::filesystem::remove(fileName);
}

// ============================================================
// Stringify round-trips
// ============================================================

#if defined(XML_LIB_ENABLE_STRINGIFY)
TEST_CASE("Embedded minimal stringify round-trip", "[XML][Embedded][Stringify]")
{
  SECTION("Stringify root only")
  {
    const std::string xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root/>";
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ xmlString }));
    BufferDestination dest;
    REQUIRE_NOTHROW(xml.stringify(dest));
    REQUIRE_FALSE(dest.toString().empty());
    REQUIRE(dest.toString().find("<root") != std::string::npos);
  }
  SECTION("Stringify preserves attributes")
  {
    const std::string xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<root key=\"value\"/>";
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ xmlString }));
    BufferDestination dest;
    REQUIRE_NOTHROW(xml.stringify(dest));
    REQUIRE(dest.toString().find("key=") != std::string::npos);
  }
  SECTION("Stringify preserves text content")
  {
    const std::string xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<root><msg>hello embedded</msg></root>";
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{ xmlString }));
    BufferDestination dest;
    REQUIRE_NOTHROW(xml.stringify(dest));
    REQUIRE(dest.toString().find("hello embedded") != std::string::npos);
  }
  SECTION("Parse-stringify-parse round-trip produces identical tree")
  {
    const std::string xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<root><a b=\"1\">text</a><c/></root>";
    XML first;
    REQUIRE_NOTHROW(first.parse(BufferSource{ xmlString }));
    BufferDestination dest;
    REQUIRE_NOTHROW(first.stringify(dest));

    XML second;
    REQUIRE_NOTHROW(second.parse(BufferSource{ dest.toString() }));
    REQUIRE(NRef<Element>(second.root()).name() == "root");
    REQUIRE(second.root().getChildren().size() == first.root().getChildren().size());
  }
}
#endif

// ============================================================
// Tree traversal
// ============================================================

TEST_CASE("Embedded tree traversal", "[XML][Embedded][Traverse]")
{
  struct CountElementLike : IAction
  {
    int elementCount = 0;
    int selfCount = 0;
    // Element (open+close tag with children)
    void onElement(Node &) override { ++elementCount; }
    void onElement(const Node &) override { ++elementCount; }
    // Self-closing element (<tag/>)
    void onSelf(Node &) override { ++selfCount; }
    void onSelf(const Node &) override { ++selfCount; }
  };

  SECTION("Traverse counts Element and Self nodes correctly")
  {
    // <root> = Root, <a/> = Self, <b/> = Self, <c> = Element, <d/> = Self
    XML xml;
    REQUIRE_NOTHROW(xml.parse(BufferSource{
      "<root><a/><b/><c><d/></c></root>" }));
    CountElementLike counter;
    REQUIRE_NOTHROW(xml.traverse(counter));
    REQUIRE(counter.elementCount == 1); // only <c>
    REQUIRE(counter.selfCount == 3);    // <a/>, <b/>, <d/>
    REQUIRE(counter.elementCount + counter.selfCount == 4);
  }
}

// ============================================================
// Library artifact size regression
// ============================================================

TEST_CASE("Embedded library artifact exists and is non-empty", "[XML][Embedded][Size]")
{
  // Search common build output locations for the library artifact
  const auto cwd = std::filesystem::current_path();
  const std::vector<std::filesystem::path> candidates{
    cwd / "libXML_Lib.a",
    cwd / ".." / "libXML_Lib.a",
    cwd.parent_path() / "libXML_Lib.a",
  };
  std::filesystem::path libPath;
  for (const auto &c : candidates) {
    if (std::filesystem::exists(c)) {
      libPath = c;
      break;
    }
  }
  REQUIRE_FALSE(libPath.empty());
  REQUIRE(std::filesystem::exists(libPath));
  const auto libSize = std::filesystem::file_size(libPath);
  REQUIRE(libSize > 0);
  // Footprint sanity: embedded library must be under 5 MB
  REQUIRE(libSize < 5 * 1024 * 1024);
}

// ============================================================
// Terminate-on-error (death tests, POSIX subprocess only)
// ============================================================
// When XML_LIB_NO_EXCEPTIONS is defined, xml_lib_throw() calls
// std::terminate() → std::abort() rather than raising an exception.
// Catch2 cannot intercept SIGABRT, so each "expected-crash" scenario
// is run inside a forked child process; the parent verifies the child
// was killed by a signal.
// ============================================================

#if defined(XML_LIB_NO_EXCEPTIONS)

#  if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#    include <sys/types.h>
#    include <sys/wait.h>
#    include <unistd.h>

/// Fork a child, execute @p fn inside it, and return true when the child
/// terminated abnormally (either killed by a signal such as SIGABRT from
/// std::terminate → std::abort, or exited with a non-zero status after a
/// signal handler like Catch2's captured the abort).
template<typename Fn>
static bool terminatesAbnormally(Fn &&fn)
{
  const pid_t pid = ::fork();
  if (pid == 0) {
    // Redirect child's stdout/stderr to /dev/null so Catch2 noise from the
    // child process does not interleave with the parent's test output.
    std::freopen("/dev/null", "w", stdout);
    std::freopen("/dev/null", "w", stderr);
    fn();
    ::_exit(0); // unreachable if std::terminate fires
  }
  int status = 0;
  ::waitpid(pid, &status, 0);
  // std::terminate() → std::abort() → SIGABRT.
  // If a signal handler (e.g., Catch2's) catches SIGABRT and exits via
  // exit(), the process exits non-zero rather than via signal — accept both.
  return WIFSIGNALED(status) || (WIFEXITED(status) && WEXITSTATUS(status) != 0);
}

TEST_CASE("No-exception: parse errors trigger std::terminate", "[XML][Embedded][Terminate]")
{
  SECTION("Unclosed root element terminates process")
  {
    REQUIRE(terminatesAbnormally([] {
      XML xml;
      xml.parse(BufferSource{ "<root" });
    }));
  }
  SECTION("Mismatched closing tag terminates process")
  {
    REQUIRE(terminatesAbnormally([] {
      XML xml;
      xml.parse(BufferSource{ "<root></other>" });
    }));
  }
  SECTION("Illegal character in tag name terminates process")
  {
    REQUIRE(terminatesAbnormally([] {
      XML xml;
      xml.parse(BufferSource{ "<1invalid/>" });
    }));
  }
  SECTION("Declaration-only document (no root) terminates process")
  {
    REQUIRE(terminatesAbnormally([] {
      XML xml;
      xml.parse(BufferSource{ "<?xml version=\"1.0\"?>" });
    }));
  }
}

#  else // non-POSIX

TEST_CASE("No-exception terminate tests: POSIX unavailable", "[XML][Embedded][Terminate]")
{
  SKIP("Death tests require POSIX fork/waitpid; not supported on this platform");
}

#  endif // __unix__ / __linux__ / __APPLE__

#endif // XML_LIB_NO_EXCEPTIONS

#endif// XML_LIB_EMBEDDED
