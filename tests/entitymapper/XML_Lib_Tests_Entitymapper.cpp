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
  //   SECTION("Create external reference entity mapping.", "[XML][EntityMapper][API]")
  // {
  //   XML_EntityMapper entityMapper;
  //   XML_ExternalReference  
  //   entityMapper.setExternal("&test;", );
  //   REQUIRE_FALSE(!entityMapper.isPresent("&test;"));
  //   REQUIRE_FALSE(!entityMapper.isNotation("&test;"));
  //   REQUIRE(entityMapper.getNotation("&test;") == "test cotents");
  // }
}