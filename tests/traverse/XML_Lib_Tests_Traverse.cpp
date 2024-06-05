
//
// Unit Tests: XML_Lib_Tests_Traverse
//
// Description: Traversal unit tests for XML class
// using the Catch2 test framework.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

class XML_Analyzer final : public IAction
{
public:
  XML_Analyzer() = default;
  ~XML_Analyzer() override = default;
  // Add JNode details to analysis
  void onXNode([[maybe_unused]] const XNode &xNode) override { totalNodes++; }
  void onCDATA([[maybe_unused]] const XNode &xNode) override { totalCDATA++; }
  void onComment([[maybe_unused]] const XNode &xNode) override { totalComment++; }
  void onContent([[maybe_unused]] const XNode &xNode) override { totalContent++; }
  void onDeclaration([[maybe_unused]] const XNode &xNode) override { totalDeclaration++; }
  void onDTD([[maybe_unused]] const XNode &xNode) override { totalDTD++; }
  void onElement([[maybe_unused]] const XNode &xNode) override { totalElement++; }
  void onEntityReference([[maybe_unused]] const XNode &xNode) override { totalEntityReference++; }
  void onPI([[maybe_unused]] const XNode &xNode) override { totalPI++; }
  void onProlog([[maybe_unused]] const XNode &xNode) override { totalProlog++; }
  void onRoot([[maybe_unused]] const XNode &xNode) override { totalRoot++; }
  void onSelf([[maybe_unused]] const XNode &xNode) override { totalSelf++; }

  // XML analysis data
  // Node
  int64_t totalNodes{};
  // CDATA
  int64_t totalCDATA{};
  // Comment
  int64_t totalComment{};
  // Content
  int64_t totalContent{};
  // Declaration
  int64_t totalDeclaration{};
  // DTD
  int64_t totalDTD{};
  // Element
  int64_t totalElement{};
  // Entity Reference
  int64_t totalEntityReference{};
  // PI
  int64_t totalPI{};
  // Prolog
  int64_t totalProlog{};
  // Root
  int64_t totalRoot{};
  // Self
  int64_t totalSelf{};
};

TEST_CASE("XML XNode tree traverse tests ", "[XML][Traverse]")
{
  const XML xml;
  SECTION("Parse minimum XML (root and no prolog) and traverse", "[XML][Traverse][Minumum]")
  {
    BufferSource source{ "<root></root>" };
    xml.parse(source);
    XML_Analyzer analyzer;
    xml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 3);
    REQUIRE(analyzer.totalCDATA == 0);
    REQUIRE(analyzer.totalComment == 0);
    REQUIRE(analyzer.totalContent == 0);
    REQUIRE(analyzer.totalDeclaration == 1);
    REQUIRE(analyzer.totalDTD == 0);
    REQUIRE(analyzer.totalElement == 0);
    REQUIRE(analyzer.totalEntityReference == 0);
    REQUIRE(analyzer.totalPI == 0);
    REQUIRE(analyzer.totalProlog == 1);
    REQUIRE(analyzer.totalRoot == 1);
    REQUIRE(analyzer.totalSelf == 0);
  }
  SECTION("Parse minimum XML (root and no prolog and content) and traverse", "[XML][Traverse][Minumum]")
  {
    BufferSource source{ "<root> test content </root>" };
    xml.parse(source);
    XML_Analyzer analyzer;
    xml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 4);
    REQUIRE(analyzer.totalCDATA == 0);
    REQUIRE(analyzer.totalComment == 0);
    REQUIRE(analyzer.totalContent == 1);
    REQUIRE(analyzer.totalDeclaration == 1);
    REQUIRE(analyzer.totalDTD == 0);
    REQUIRE(analyzer.totalElement == 0);
    REQUIRE(analyzer.totalEntityReference == 0);
    REQUIRE(analyzer.totalPI == 0);
    REQUIRE(analyzer.totalProlog == 1);
    REQUIRE(analyzer.totalRoot == 1);
    REQUIRE(analyzer.totalSelf == 0);
  }
  SECTION("Parse minimum XML (root and no prolog, content, an entity reference and comments) and traverse", "[XML][Traverse][Minumum]")
  {
    BufferSource source{
      "<!-- comment 1 -->"
      "<root> test content <!-- comment 2 --></root>"
    };
    xml.parse(source);
    XML_Analyzer analyzer;
    xml.traverse(analyzer);
    REQUIRE(analyzer.totalNodes == 6);
    REQUIRE(analyzer.totalCDATA == 0);
    REQUIRE(analyzer.totalComment == 2);
    REQUIRE(analyzer.totalContent == 1);
    REQUIRE(analyzer.totalDeclaration == 1);
    REQUIRE(analyzer.totalDTD == 0);
    REQUIRE(analyzer.totalElement == 0);
    REQUIRE(analyzer.totalEntityReference == 0);
    REQUIRE(analyzer.totalPI == 0);
    REQUIRE(analyzer.totalProlog == 1);
    REQUIRE(analyzer.totalRoot == 1);
    REQUIRE(analyzer.totalSelf == 0);
  }
  // SECTION("Parse minimum XML (root and no prolog, an entity references and traverse", "[XML][Traverse][Minumum]")
  // {
  //   BufferSource source{
  //     "<root> &apos; </root>"
  //   };
  //   xml.parse(source);
  //   XML_Analyzer analyzer;
  //   xml.traverse(analyzer);
  //   REQUIRE(analyzer.totalNodes == 7);
  //   REQUIRE(analyzer.totalCDATA == 0);
  //   REQUIRE(analyzer.totalComment == 0);
  //   REQUIRE(analyzer.totalContent == 2);
  //   REQUIRE(analyzer.totalDeclaration == 1);
  //   REQUIRE(analyzer.totalDTD == 0);
  //   REQUIRE(analyzer.totalElement == 0);
  //   REQUIRE(analyzer.totalEntityReference == 1);
  //   REQUIRE(analyzer.totalPI == 0);
  //   REQUIRE(analyzer.totalProlog == 1);
  //   REQUIRE(analyzer.totalRoot == 1);
  //   REQUIRE(analyzer.totalSelf == 0);
  // }
  // SECTION("Parse minimum XML (root and no prolog, content, two entity references and comments) and traverse", "[XML][Traverse][Minumum]")
  // {
  //   BufferSource source{
  //     "<!-- comment 1 -->"
  //     "<root> &apos;test content&apos; <!-- comment 2 --></root>"
  //   };
  //   xml.parse(source);
  //   XML_Analyzer analyzer;
  //   xml.traverse(analyzer);
  //   REQUIRE(analyzer.totalNodes == 12);
  //   REQUIRE(analyzer.totalCDATA == 0);
  //   REQUIRE(analyzer.totalComment == 2);
  //   REQUIRE(analyzer.totalContent == 1);
  //   REQUIRE(analyzer.totalDeclaration == 1);
  //   REQUIRE(analyzer.totalDTD == 0);
  //   REQUIRE(analyzer.totalElement == 0);
  //   REQUIRE(analyzer.totalEntityReference == 0);
  //   REQUIRE(analyzer.totalPI == 0);
  //   REQUIRE(analyzer.totalProlog == 1);
  //   REQUIRE(analyzer.totalRoot == 1);
  //   REQUIRE(analyzer.totalSelf == 0);
  // }
}