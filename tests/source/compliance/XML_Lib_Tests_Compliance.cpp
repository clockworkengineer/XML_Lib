#include "XML_Lib_ests.hpp"

TEST_CASE("Compliance fixture infrastructure", "[Compliance][Infrastructure]")
{
  SECTION("Test data path resolves to the repository test file root.", "[Compliance][Infrastructure]")
  {
    const auto expectedSuffix = std::string("tests/files/w3c/xml/placeholder.xml");
    const auto path = prefixTestDataPath("w3c/xml/placeholder.xml");
    REQUIRE(path.find(expectedSuffix) != std::string::npos);
  }
}
    