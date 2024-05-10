//
// Unit Tests: XML_Lib_Tests_Entitymapper
//
// Description: XML Entity Mapper unit tests.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("XML entity mapper useage tests cases.", "[XML][EntityMapper]")
{
  SECTION("Create entity mapper does not throw execption..", "[XML][PaEntityMapperrse][Create]")
  {
    REQUIRE_NOTHROW(XML_EntityMapper());
  }
  SECTION("Create entity mapper intialised default with &amp; entry.", "[XML][EntityMapper][DefaultValues]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(!entityMapper.isPresent("&amp;"));
    REQUIRE_FALSE(!entityMapper.isInternal("&amp;"));
    REQUIRE(entityMapper.getInternal("&amp;") == "&#x26;");
  }
  SECTION("Create entity mapper intialised default with &quot; entry.", "[XML][EntityMapper][DefaultValues]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(!entityMapper.isPresent("&quot;"));
    REQUIRE_FALSE(!entityMapper.isInternal("&quot;"));
    REQUIRE(entityMapper.getInternal("&quot;") == "&#x22;");
  }
  SECTION("Create entity mapper intialised default with &apos; entry.", "[XML][EntityMapper][DefaultValues]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(!entityMapper.isPresent("&apos;"));
    REQUIRE_FALSE(!entityMapper.isInternal("&apos;"));
    REQUIRE(entityMapper.getInternal("&apos;") == "&#x27;");
  }
  SECTION("Create entity mapper intialised default with &lt; entry.", "[XML][EntityMapper][DefaultValues]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(!entityMapper.isPresent("&lt;"));
    REQUIRE_FALSE(!entityMapper.isInternal("&lt;"));
    REQUIRE(entityMapper.getInternal("&lt;") == "&#x3C;");
  }
  SECTION("Create entity mapper intialised default with &gt; entry.", "[XML][EntityMapper][DefaultValues]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(!entityMapper.isPresent("&gt;"));
    REQUIRE_FALSE(!entityMapper.isInternal("&gt;"));
    REQUIRE(entityMapper.getInternal("&gt;") == "&#x3E;");
  }
  SECTION("Try to see if a non-existant entry exists.", "[XML][EntityMapper][API]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(entityMapper.isPresent("&test;"));
  }
  SECTION("Try to see if a non-existant entry is internal.", "[XML][EntityMapper][API]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(entityMapper.isInternal("&test;"));
  }
  SECTION("Try to access emmpty entry (not been set).", "[XML][EntityMapper][API]")
  {
    XML_EntityMapper entityMapper;
    REQUIRE_FALSE(entityMapper.isPresent("&test;"));
    REQUIRE_THROWS_WITH(
      entityMapper.getInternal("&test;"), "IEntityMapper Error: Internal entity reference not found for '&test;'.");
  }
  SECTION("Create internal entity mapping.", "[XML][EntityMapper][API]")
  {
    XML_EntityMapper entityMapper;
    entityMapper.setInternal("&test;", "test cotents");
    REQUIRE_FALSE(!entityMapper.isPresent("&test;"));
    REQUIRE_FALSE(!entityMapper.isInternal("&test;"));
    REQUIRE(entityMapper.getInternal("&test;") == "test cotents");
  }
  SECTION("Create notation entity mapping.", "[XML][EntityMapper][API]")
  {
    XML_EntityMapper entityMapper;
    entityMapper.setNotation("&test;", "test cotents");
    REQUIRE_FALSE(!entityMapper.isPresent("&test;"));
    REQUIRE_FALSE(!entityMapper.isNotation("&test;"));
    REQUIRE(entityMapper.getNotation("&test;") == "test cotents");
  }
  SECTION("Create system external reference entity mapping.", "[XML][EntityMapper][API]")
  {
    XML_EntityMapper entityMapper;
    XML_ExternalReference externalReference{ "SYSTEM", "chap1.xml" };
    entityMapper.setExternal("&test;", externalReference);
    REQUIRE_FALSE(!entityMapper.isPresent("&test;"));
    REQUIRE_FALSE(!entityMapper.isExternal("&test;"));
    REQUIRE(entityMapper.getExternal("&test;").getSystemID() == "chap1.xml");
  }
  SECTION("Create public external reference entity mapping.", "[XML][EntityMapper][API]")
  {
    XML_EntityMapper entityMapper;
    XML_ExternalReference externalReference{
      "PUBLIC", "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd", "-//W3C//DTD XHTML 1.0 Transitional//EN"
    };
    entityMapper.setExternal("&test;", externalReference);
    REQUIRE_FALSE(!entityMapper.isPresent("&test;"));
    REQUIRE_FALSE(!entityMapper.isExternal("&test;"));
    REQUIRE(entityMapper.getExternal("&test;").getPublicID() == "-//W3C//DTD XHTML 1.0 Transitional//EN");
    REQUIRE(
      entityMapper.getExternal("&test;").getSystemID() == "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
  }
}