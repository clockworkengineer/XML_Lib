#include "XML_Lib_Tests.hpp"

TEST_CASE("Parser security corpus files fail safely", "[XML][Security][Corpus]")
{
  const std::vector<std::string> securityFiles{
    "security/malformed_unclosed_tag.xml",
    "security/malformed_unterminated_comment.xml",
    "security/malformed_entity_unknown.xml",
    "security/malformed_xxe_external_entity.xml",
    "security/malformed_attribute_quote.xml"
  };

  for (const auto &securityFile : securityFiles) {
    const std::string filePath = prefixTestDataPath(securityFile);
    INFO("Testing security corpus file: " << securityFile);
    const std::string xmlText = XML::fromFile(filePath);
    XML xml;
    REQUIRE_THROWS(xml.parse(BufferSource{ xmlText }));
  }
}
