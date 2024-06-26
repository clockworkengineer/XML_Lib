
#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"

class XML_Analyzer final : public XML_Lib::IAction
{
public:
  XML_Analyzer() = default;
  ~XML_Analyzer() override = default;
  // Add XNode details to analysis
  void onXNode([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalNodes++;
    sizeInBytes += sizeof(XML_Lib::XNode);
  }
  void onCDATA([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalCDATA++;
    sizeInBytes += sizeof(XML_Lib::XCDATA) + XML_Lib::XRef<XML_Lib::XCDATA>(xNode).value().size();
  }
  void onComment([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalComment++;
    sizeInBytes += sizeof(XML_Lib::XComment)+ XML_Lib::XRef<XML_Lib::XComment>(xNode).value().size();
  }
  void onContent([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalContent++;
    sizeInBytes += sizeof(XML_Lib::XContent)+ XML_Lib::XRef<XML_Lib::XContent>(xNode).value().size();
  }
  void onDeclaration([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalDeclaration++;
    sizeInBytes += sizeof(XML_Lib::XDeclaration);
  }
  void onDTD([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalDTD++;
    sizeInBytes += sizeof(XML_Lib::XDTD);
  }
  void onElement([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalElement++;
    sizeInBytes += sizeof(XML_Lib::XElement);
  }
  void onEntityReference([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalEntityReference++;
    sizeInBytes += sizeof(XML_Lib::XEntityReference);
  }
  void onPI([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalPI++;
    sizeInBytes += sizeof(XML_Lib::XPI);
  }
  void onProlog([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalProlog++;
    sizeInBytes += sizeof(XML_Lib::XProlog);
  }
  void onRoot([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalRoot++;
    sizeInBytes += sizeof(XML_Lib::XRoot);
  }
  void onSelf([[maybe_unused]] const XML_Lib::XNode &xNode) override
  {
    totalSelf++;
    sizeInBytes += sizeof(XML_Lib::XSelf);
  }
  // Output analysis details
  [[nodiscard]] std::string dump() const
  {
    std::stringstream os;
    os << "\n------------------XML Tree Stats------------------\n";
    os << "XML Tree contains " << totalNodes << " nodes.\n";
    os << "XML Tree size " << sizeInBytes << " in bytes.\n";
    os << "----------------------------------------------------";
    return (os.str());
  }
  static std::string dumpXNodeSizes()
  {
    std::stringstream os;
    os << "\n--------------------XML_Lib::XNode Sizes---------------------\n";
    os << "XML_Lib::XMLAttribute size " << sizeof(XML_Lib::XMLAttribute) << " in bytes.\n";
    os << "XML_Lib::XMLExternalReference size " << sizeof(XML_Lib::XMLExternalReference) << " in bytes.\n";
    os << "XML_Lib::XMLValue size " << sizeof(XML_Lib::XMLValue) << " in bytes.\n";
    os << "XML_Lib::XNode size " << sizeof(XML_Lib::XNode) << " in bytes.\n";
    os << "XML_Lib::XCDATA size " << sizeof(XML_Lib::XCDATA) << " in bytes.\n";
    os << "XML_Lib::XComment Entry size " << sizeof(XML_Lib::XComment) << " in bytes.\n";
    os << "XML_Lib::XContent size " << sizeof(XML_Lib::XContent) << " in bytes.\n";
    os << "XML_Lib::XDeclaration size " << sizeof(XML_Lib::XDeclaration) << " in bytes.\n";
    os << "XML_Lib::XDTD size " << sizeof(XML_Lib::XDTD) << " in bytes.\n";
    os << "XML_Lib::XElement size " << sizeof(XML_Lib::XElement) << " in bytes.\n";
    os << "XML_Lib::XEntityReference size " << sizeof(XML_Lib::XEntityReference) << " in bytes.\n";
    os << "XML_Lib::XPI size " << sizeof(XML_Lib::XPI) << " in bytes.\n";
    os << "XML_Lib::XProlog size " << sizeof(XML_Lib::XProlog) << " in bytes.\n";
    os << "XML_Lib::XRoot size " << sizeof(XML_Lib::XRoot) << " in bytes.\n";
    os << "XML_Lib::XSelf size " << sizeof(XML_Lib::XSelf) << " in bytes.\n";
    return (os.str());
  }
  static std::string dumpNumericSizes()
  {
    std::stringstream os;
    os << "\n--------------------System Numeric Sizes---------------------\n";
    os << "Short size " << sizeof(short) << " in bytes.\n";
    os << "Int size " << sizeof(int) << " in bytes.\n";
    os << "Long Long size " << sizeof(long long) << " in bytes.\n";
    os << "Float size " << sizeof(float) << " in bytes.\n";
    os << "Double size " << sizeof(double) << " in bytes.\n";
    os << "Long Double size " << sizeof(long double) << " in bytes.\n";
    return (os.str());
  }

private:
  // XML analysis data
  // XNode
  int64_t totalNodes{};
  size_t sizeInBytes{};
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